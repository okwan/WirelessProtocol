#include "Physical_Read.h"

HANDLE hComm;
HANDLE hRead_Lock = CreateMutex(NULL, FALSE, READ_LOCK);
HANDLE Ev_Read_Thread_Finish = CreateEvent(NULL, TRUE, TRUE, 0);
BOOL receivedENQinWait;

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Initialize_Serial_Port
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL Initialize_Serial_Port();
--
-- RETURNS: VOID
--
-- NOTES:
--		Initialize serial communication handle to allow overlapped communication
----------------------------------------------------------------------------------------------------------------------*/
VOID Initialize_Serial_Port()
{
	Error_Check((hComm = CreateFile(lpszCommName, 
		GENERIC_READ | GENERIC_WRITE, 
		0,
		NULL, 
		OPEN_EXISTING, 
		FILE_FLAG_OVERLAPPED,				// File overlapped flag to allow asynchrounous operations
		NULL)) == INVALID_HANDLE_VALUE ?
		ERR_INIT_COMM : NO_ERR);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Initialize_Read
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: VOID Initialize_Read()
--
-- RETURNS: VOID
--
-- NOTES:
--		Initialize the read operation and reports any error if it occurs
----------------------------------------------------------------------------------------------------------------------*/
VOID Initialize_Read()
{
	Error_Check((rThread = CreateThread(NULL, 0, Read_Idle, NULL , 0, &rThreadId)) == NULL ?
		ERR_READ_THREAD : NO_ERR);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Read_Idle
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: DWORD WINAPI Read_Idle(LPVOID lpvoid)
--
-- RETURNS: 0 upon successful thread finish
--
-- NOTES:
--		Represents the idle state of the wireless protocol design. Basically waits forever until an ENQ or DC2 is read 
--	from the serial port. It sends an either ACK or DC1 back to the port depending on the priority status of the system,
--  and then waits for a packet to come back after. Finally, the thread returns to the idle state and waits for the ENQ
-- or DC2 to arrive again.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Read_Idle(LPVOID lpvoid)
{	
	DWORD total_bytes_read;
	// til the end of time
	while (1)
	{
		// If we got an ENQ already from the wait state, go directly to sending an ACK to acknowledge line
		if (receivedENQinWait) {
			ResetEvent(Ev_Read_Thread_Finish);
			// Clear the flag
			receivedENQinWait = false;
			// Send a DC1 | ACK
			Send_Response();
			// Now we wait for the packet to come, and validate it
			Wait_For_Packet();
		} 
		else
		{
			// Idle state waiting
			char *str = Wait_For_Input();
			ResetEvent(Ev_Read_Thread_Finish);
			// If we get an ENQ | DC2
			if (Evaluate_Input(str[0]))
			{
				// Send a DC1 | ACK
				Send_Response();
				// Now we wait for the packet to come, and validate it
				Wait_For_Packet();
			}
		}
	}
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Send_Response
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: VOID Send_Response()
--
-- RETURNS: VOID
--
-- NOTES:
--		Sends a DC1 or ACK to the serial port depending on the current priority status of the system.
----------------------------------------------------------------------------------------------------------------------*/
VOID Send_Response()
{
	updateStats(stats.acksReceived++, IDC_SDATA4);
	char  response[1];
	// Checks which response we should send back
	response[0] = weHavePriority ? DC1 : ACK;
	OutputDebugString("Sending a response\n");
	Send(response, sizeof(response), hRead_Lock);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Wait_For_Input
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: LPSTR Wait_For_Input()
--
-- RETURNS: long pointer to the char that has been received from the port
--
-- NOTES:
--		Use event driven approach to wait for ENQ | DC2. Used only by the Read_Idle() thread. Also handles the mechanism 
--	to exit the thread properly when performing WaitCommEvent()
----------------------------------------------------------------------------------------------------------------------*/
LPSTR Wait_For_Input()
{
	COMSTAT cs;
	char str[1];
	OVERLAPPED ovRead = { NULL };
	// Create overlapped event
	ovRead.hEvent = CreateEvent(NULL, FALSE, FALSE, EV_OVREAD);
	//	Set listener to a character
	Error_Check(!SetCommMask(hComm, EV_RXCHAR) ? ERR_COMMMASK : NO_ERR);
	DWORD read_byte, dwEvent, dwError;
	// Waits for EV_RXCHAR event to trigger
	if (WaitCommEvent(hComm, &dwEvent, NULL))
	{
		// Clear Comm Port
		ClearCommError(hComm, &dwError, &cs);
		if ((dwEvent & EV_RXCHAR) && cs.cbInQue)
		{
			if (!ReadFile(hComm, str, sizeof(str), &read_byte, &ovRead))
				Output_GetLastError();
		}
	}
	// If a event other than EV_RXCHAR has occured, which should be triggered by the RETURN_COMM_EVENT macro.
	if (dwEvent == 0)
		// End the read thread
		ExitThread(rThreadId);
	// Discards all characters from the output and input buffer
	PurgeComm(hComm, PURGE_RXCLEAR | PURGE_TXCLEAR);
	CloseHandle(ovRead.hEvent);
	return str;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Evaluate_Input
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  Evaluate_Input(CHAR c)
--					-CHAR c : the character to be evaluated 
--
-- RETURNS: TRUE if c is an ENQ or DC2, false otherwise
--
-- NOTES:
--		Use event driven approach to wait for ENQ | DC2. Used only by the Read_Idle() thread. Also handles the mechanism
--	to exit the thread properly when performing WaitCommEvent()
----------------------------------------------------------------------------------------------------------------------*/
BOOL Evaluate_Input(CHAR c)
{
	if (c != ENQ && c != DC2)
		return FALSE;
	if (c == ENQ)
	{
		OutputDebugString("Got an Enq\n");
		senderHasPriority = FALSE;
		return TRUE;
	}
	else if (c == DC2)
	{
		OutputDebugString("Got an DC2\n");
		senderHasPriority = TRUE;
		return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Wait_For_Packet
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  VOID Wait_For_Packet()
--
-- RETURNS: VOID
--
-- NOTES:
--		Keeps on waiting for characters arrive at the serial port and attempt to build them into a packet
----------------------------------------------------------------------------------------------------------------------*/
VOID Wait_For_Packet()
{
	BOOL  packet_recieved = FALSE;
	while (!packet_recieved)
	{
		char *str;
		OutputDebugString("Waiting for packet\n");
		// If timeout waiting for packet
		if (!Wait_For_Data(&str, 516, 3000))
		{
			// Check priority state
			// go back to idle or wait state
			//only when we dont have priority and sender has priority, we go to wait state
			if (!weHavePriority && senderHasPriority) {
				//WAIT STATE: wait for an enq, for a specified amount of time. If we get one, set
				//the receivedENQinWait boolean flag to skip directly to acknowledging line in read thread
				OutputDebugString("Going to wait state\n");
				receivedENQinWait = WaitForEnq();
			}

			OutputDebugString("Going back to idle\n");
			SetEvent(Ev_Read_Thread_Finish);
			return;
		}

		// If an EOT has been recieved (end of packet) or 516 characters has been recieveds
		if (str[strlen(str)-1] == EOT || strlen(str) >= MAX_PACKET_SIZE)
		{
			//if packet good, we send an ack | dc1, else we continue to wait for a new packet
			if (Check_Packet(str))
			{
				// get outta da loop
				OutputDebugString("Valid Packet!\n");
				packet_recieved = true;
			}
			else
			{
				OutputDebugString("Invalid Packet!\n");
			}
		}
	}
	// send ACK | DC2 to confirm a valid packet
	Send_Response();
	if (!weHavePriority && senderHasPriority) {
		//WAIT STATE: wait for an enq, for a specified amount of time. If we get one, set
		//the receivedENQinWait boolean flag to skip directly to acknowledging line in read thread
		OutputDebugString("Going to wait state\n");
		receivedENQinWait = WaitForEnq();
	}
	SetEvent(Ev_Read_Thread_Finish);
	SetEvent(Ev_Send_Thread_Finish);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Check_Packet
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL Check_Packet(const char *packet)
--				-packet : char array to validate
--
-- RETURNS: TRUE upon valid packet, FALSE otherwise
--
-- NOTES:
--		Validate packet, make sure the packet makeup is correctly and that the packet is in sync
----------------------------------------------------------------------------------------------------------------------*/
BOOL Check_Packet(const char *packet)
{
	size_t i;
	std::string message;
	OutputDebugString("In packet check\n");
	if (packet[0] == SOH)
		OutputDebugString("valid SOH\n");
	if (packet[1] == SYN1 || packet[1] == SYN0)
		OutputDebugString("valid SYN1 and SYN0\n");
	// Make sure the first 2 characters are in place
	if (packet[0] == SOH && (packet[1] == SYN1 || packet[1] == SYN0))
	{
		// Check if it is a redundent packet
		if (packet[1] == prev_sync_received) {
			OutputDebugString("Sync byte check failed, received the same sync byte.");
			return FALSE;
		}

		prev_sync_received = packet[1];

		for (i = 4; i < MAX_PACKET_SIZE; i++)
		{
			if (packet[i] == EOT)
			{
				Read_Packets.push_back(message);
				/* 
				*	signal end of transmission of file. de-packetize Read_Packets vector
				*	and build it into a file
				*/
				for (auto packet : Read_Packets)
					OutputDebugString(packet.c_str());
				Read_Packets.clear();
				prev_sync_received = NULL;
				break;
			}
			message += packet[i];

		}
		updateStats(stats.packetReceived++, IDC_SDATA2);
		addLine(&sendTo, message);
		Read_Packets.push_back(message);
		return TRUE;

	}
	return FALSE;
}
