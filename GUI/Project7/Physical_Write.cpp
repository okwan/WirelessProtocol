#include "Physical_Write.h"

HANDLE hWrite_Lock = CreateMutex(NULL, FALSE, WRITE_LOCK);
HANDLE Ev_Send_Thread_Finish = CreateEvent(NULL, TRUE, FALSE, NULL);

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Start_Packet_Loader_Thread
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  DWORD WINAPI Start_Packet_Loader_Thread(LPVOID lpvoid)
--
-- RETURNS: DWORD thread exit code
--
-- NOTES:
--		Packetize file opened on the GUI and feeds each packets into the write engine. Since we have to wait until
--      each packets to finish sending before sending the next one, we have to allocate a thread for this procedure or
--	    else it will hang in the main thread.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Start_Packet_Loader_Thread(LPVOID lpvoid)
{
	Write_Packets = Packetize_Data();

	for (auto packet : Write_Packets)
	{
		char *tmp = new char[packet.length() + 1];
		*tmp = 0;
		strncat(tmp, packet.c_str(), packet.length()+1);
		Initialize_Write(tmp);
		WaitForSingleObject(Ev_Read_Thread_Finish, 2000);
		OutputDebugString("[WRITING NEXT PACKET]\n");
		ResetEvent(Ev_Read_Thread_Finish);
		// stats sendPackets
		updateStats(stats.packetSent++, IDC_SDATA0);
	}
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Initialize_Write
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  Initialize_Write(LPBYTE packet)
--					-char* packet : the packet to be sent
--
-- RETURNS: VOID
--
-- NOTES:
--		Initializes the write operation and reports any errors if it occurs
----------------------------------------------------------------------------------------------------------------------*/
VOID Initialize_Write(char* packet)
{

	Error_Check((wThread = CreateThread(NULL, 0, Transfer_Packet, (LPVOID)packet, 0, &wThreadId)) == NULL ?
		ERR_WRITE_THREAD : NO_ERR);
	WaitForSingleObject(Ev_Send_Thread_Finish, INFINITE);
	ResetEvent(Ev_Send_Thread_Finish);
}


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Transfer_Packet
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  Transfer_Packet(LPVOID packet)
--					-LPVOID packet : the packet to be sent
--
-- RETURNS: 0 upon successful thread finish
--
-- NOTES:
--		Represents the sending state of the wireless protocol design.  This thread is called whenever a user chooses a
--  file to send and presses the Send button.  We confirm the line to make sure the line is available.  If confirm line
--  times out, we bring the protocol back to the Wait state to wait for an ENQ, and re-initialize the read thread.  If
--  confirm line succeeds, we send our packet using SendPacket.  When SendPacket succeeds or times out, we do a check
--  on the priority states of the sender and receiver.  If we don't have priority and the sender has priority, we enter
--  the wait state again to wait for an ENQ, otherwise we will just go directly to the read idle state.  Finally, we
--  reinitialize the read thread and return from this write thread.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Transfer_Packet(LPVOID packet)
{
	ResetEvent(Ev_Read_Thread_Finish);
	SetCommMask(hComm, RETURN_COMM_EVENT);
	WaitForSingleObject(hWrite_Lock, 2000);
	char* s = (char*)packet;
	OutputDebugString("Starting confirm line loop\n");
	if (!ConfirmLine()) 
	{
		OutputDebugString("exceeded confirm line max tries\n");

		//WAIT STATE: wait for an enq, for a specified amount of time. If we get one, set
		//the receivedENQinWait boolean flag to skip directly to acknowledging line in read thread
		receivedENQinWait = WaitForEnq();
		ReleaseMutex(hWrite_Lock);
		Initialize_Read();
		SetEvent(Ev_Send_Thread_Finish);
		delete[]s;
		return 0;
	}
	OutputDebugString("Sending packet\n");
	SendPacket(s);

	//Check Priorities
	//only when we want priority and sender doesn't want priority, we go directly to read idle. every other priority combo,
	//we go to wait state
	if ((!weHavePriority && senderHasPriority) || (weHavePriority && senderHasPriority) || (!weHavePriority && !senderHasPriority))
	{
		//WAIT STATE: wait for an enq, for a specified amount of time. If we get one, set
		//the receivedENQinWait boolean flag to skip directly to acknowledging line in read thread
		OutputDebugString("Going to wait state\n");
		receivedENQinWait = WaitForEnq();
	}else 
		SetEvent(Ev_Send_Thread_Finish);
	//going back to wait/idle state
	ReleaseMutex(hWrite_Lock);
	Initialize_Read();
	delete[]s;
	return 0;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ConfirmLine
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  ConfirmLine()
--
-- RETURNS: TRUE if we were able to get an ACK for the ENQ/DC2 that we sent, or
--          FALSE if we timeout on getting an ACK, and we exceeded the maximum number of tries to confirm the line.
--
-- NOTES:
--		Sends an ENQ or DC2 (depending on whether we want priority) to the serial port, to ask to have the line so we
--  can send a packet.  If we get an ACK back, we have successfully confirmed the line, and we return true.  If we
--  timeout and don't get an ACK back, we try again, up to the maximum number of times specified.
----------------------------------------------------------------------------------------------------------------------*/
BOOL ConfirmLine() {
	DWORD numTries_confirmLine = 0;
	//Try to send an ENQ to confirm the line until we reach the maximum number of tries
	while (numTries_confirmLine < CONFIRM_LINE_MAX_TRIES) {
		//Send an ENQ or DC2 to ask for the line
		char str[1];
		
		// Checks whether we should send a DC2 / ENQ depending on whether we want priority
		str[0] = weHavePriority ? DC2 : ENQ;

		OutputDebugString("sending an ENQ or DC2\n");
		Send(str, sizeof(str), hWrite_Lock);

		//Wait for a response for the DC2 / ENQ we sent
		char *response;
		if (!Wait_For_Data(&response, 1, 500))
		{
			OutputDebugString("confirm line timeout\n");
			//Time'd out, increment counter and restart the loop to send the ENQ again
			numTries_confirmLine++;
			continue;
		}
		if (Evaluate_Response(response[0]))
			return TRUE;
	}
	return FALSE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendPacket
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  SendPacket(char* str)
--					-char* str : the packet to be sent
--
-- RETURNS: VOID
--
-- NOTES:
--		Sends a packet to the serial port.  If we get an ACK back acknowledging the packet we just sent, the packet is
--  considered sent successfully, and we return.  If we don't get an ACK back, we assume something went wrong with the
--  transmission and we resend the same packet, up to the maximum specified tries.  
----------------------------------------------------------------------------------------------------------------------*/
VOID SendPacket(char* str) { 
	DWORD numTries_sendPacket = 0;

	//Try to send the packet until we reach the maximum number of tries
	while (numTries_sendPacket < SEND_PACKET_MAX_TRIES) {
		//Send the packet
		Send(str, strlen(str), hWrite_Lock);

		//Wait for a response for the Packet we sent
		char *response = "";
		if (!Wait_For_Data(&response, 1, 3000))
		{
			updateStats(stats.packetSent++, IDC_SDATA0);

			//Time'd out, increment counter and restart the loop to send the packet again
			OutputDebugString("Packet sending timeout\n");
			numTries_sendPacket++;
			continue;
		}
		else if (Evaluate_Response (response[0])) {
			updateProgressBar (progressSize / Write_Packets.size());
			return;
		}

	}
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: FlipSyncByte
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  FlipSyncByte()
--
-- RETURNS: CHAR representing the previous sync byte
--
-- NOTES:
--		Flips the sync byte based on the sync byte in the packet that was previously received.  
----------------------------------------------------------------------------------------------------------------------*/
CHAR FlipSyncByte() {
	if (prev_sync_sent == SYN1) {
		prev_sync_sent = SYN0;
	}
	else {
		prev_sync_sent = SYN1;
	}
	return prev_sync_sent;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Evaluate_Response
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  Evaluate_Response(char c)
--
-- RETURNS: BOOL - TRUE if we either got an ACK or DC1
--               - FALSE if we don't get either of those
--
-- NOTES:
--		Checks to see if we either got an ACK or DC1 as a response, and updates priority based on which is received.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Evaluate_Response(char c)
{
	if (c == ACK || c == DC1)
	{
		OutputDebugString("Got ACK or DC1 Backk\n");
		//Update the priority states based on the response
		senderHasPriority = (c == DC1 ? true : false);
		return TRUE;
	}
	return  FALSE;
}


