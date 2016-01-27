#include "Physical.h"
BOOL senderHasPriority = FALSE;
BOOL weHavePriority = FALSE;;

FILE_STATISTICS stats;
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Wait_For_Data
--
-- DATE: November 22, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL Wait_For_Data(	char	**str,
--									DWORD	buffer_size,
--									DWORD	TIMEOUT)
--					- char **str				: Buffer to store the content retrived by Read_File
--					- DWORD buffer_size			: The size of the buffer, has to be specified explicitly
--					- DWORD TIMEOUT				: Waiting time
--					- HANDLE lock				: basically pass in hRead_Lock if you're operating in
--							Physical_Read.h, hWrite_Lock if in Physical_Write.h
--
-- RETURNS: TRUE if data read successfully, FALSE if timeout
--
-- NOTES:
--	Performs asychrounous IO read from the serial port. If no data has been recieved before timer expires the function 
--	returns false indicating a failure. If data is being detected on the serial port we start reading until 516 characters 
--  has been successfully processed or when we have read an EOT (end of transmission). If ReadFile ever fails we check the 
--  status of the serial port. if serial port is busy (indicating readfile has not finish processing previous character 
--  read) we call WaitForSingleObject() to wait until the port clear. every character read from readfile is appended onto
--  a string and after the while loop it is being copied to str.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Wait_For_Data(
	char	**str,
	DWORD	buffer_size,
	DWORD	TIMEOUT)
{
	std::string s;
	DWORD total = 0, bytes_read;
	*str = new char[buffer_size];
	OVERLAPPED ovRead = { NULL };
	ovRead.hEvent = CreateEvent(NULL, FALSE, FALSE, EV_OVREAD);
	if (!Timeout(TIMEOUT))
	{
		OutputDebugString("Timeout\n");
		return FALSE;
	}
	// while characters read are smaller than the buffer size
	while (total < buffer_size)
	{
		char tmp[1] = "";
		if (!ReadFile(hComm, tmp, 1, &bytes_read, &ovRead))
		{
			if (GetLastError() == ERROR_IO_PENDING)
				if (WaitForSingleObject(ovRead.hEvent, 50) != WAIT_OBJECT_0)
					return FALSE;
		}
		if (tmp[0] == EOT)
		{
			s += tmp[0];
			break;
		}
		s += tmp[0];
		total++;
	}
	*str = (char*)malloc(strlen(s.c_str()));
	strcpy(*str, s.c_str());


	return TRUE;
}
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Wait_For_Data
--
-- DATE: November 22, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: VOID Send(char* msg, DWORD size, HANDLE lock)
--					- char *msg		: buffer to sent to the serial port
--					- DWORD size	: size of the buffer
--					- HANDLE lock	: basically pass in hRead_Lock if you're operating in
--							Physical_Read.h, hWrite_Lock if in Physical_Write.h
--
-- RETURNS: VOID
--
-- NOTES:
--	 
--    Send a buffer of data to the serial port. Make sure that no other thread is running at the same time while
--    performing asynchrounous IO operations. For each character sent to the serial port we make sure that it is 
--    being successfully processed before sending the next one by calling WaitForSingleObject on the overlapped 
--    object.
----------------------------------------------------------------------------------------------------------------------*/

VOID Send(char* msg, DWORD size, HANDLE lock)
{
	COMSTAT cs;
	DWORD err, result, bytes_written;
	OVERLAPPED ovWrite = { NULL };
	ovWrite.hEvent = CreateEvent(NULL, FALSE, FALSE, EV_OVWRITE);
	// Lock this thread
	WaitForSingleObject(lock, INFINITE);

	if (!WriteFile(hComm, msg, size, &bytes_written, &ovWrite))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if ((result = WaitForSingleObject(ovWrite.hEvent, INFINITE)) == WAIT_OBJECT_0)
			{
				if (GetOverlappedResult(hComm, &ovWrite, &bytes_written, FALSE))
					OutputDebugString("Write Successfully\n");
				else
					OutputDebugString("Write Failed\n");
			}
			else
				OutputDebugString("Error occured in Send()::WaitForSingleObject()\n");
		}
	}
	// Release this thread
	ReleaseMutex(hWrite_Lock);
	CloseHandle(ovWrite.hEvent);
	ClearCommError(hComm, &err, &cs);
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Timeout
--
-- DATE: November 22, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL Timeout(DWORD msec)
--					DWORD msec : number of milliseconds to wait 
-- RETURNS: TRUE if EV_RXCHAR is read before timer expires, FALSE otherwise
--
-- NOTES:
--		Waits for an EV_RXCHAR event to occur on the serial port with a given amount of time.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Timeout(DWORD msec)
{
	COMSTAT cs;
	DWORD fdwCommMask, err;
	SetCommMask(hComm, EV_RXCHAR);
	OVERLAPPED OverLapped;
	memset((char *)&OverLapped, 0, sizeof(OVERLAPPED));
	OverLapped.hEvent = CreateEvent(NULL, TRUE, TRUE, 0);
	if (!WaitCommEvent(hComm, &fdwCommMask, &OverLapped))
	{
		if (GetLastError() == ERROR_IO_PENDING)
		{
			if (WaitForSingleObject(OverLapped.hEvent, (DWORD)msec) != WAIT_OBJECT_0)
				return FALSE;
		}
	}
	CloseHandle(OverLapped.hEvent);
	/*if ((fdwCommMask & EV_RXCHAR) != EV_RXCHAR)
		return FALSE;*/
	ClearCommError(hComm, &err, &cs);
	return TRUE;
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: WaitForEnq
--
-- DATE: November 22, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE: BOOL WaitForEnq()
--
-- RETURNS: TRUE if ENQ or DC2 is read before timer expires, FALSE otherwise
--
-- NOTES:
--		Wait state of the state chart protocol design. Waits for an ENQ or DC2 to arrive from the serial port with 
-- a timeout of 200 milliseconds. If ENQ or DC2 is recieved, go to acknowledge line state. if not, go to idle state
----------------------------------------------------------------------------------------------------------------------*/
BOOL WaitForEnq()
{
	char *response = "";
	if (!Wait_For_Data(&response, 1, 200))
	{
		SetEvent(Ev_Send_Thread_Finish);
		SetEvent(Ev_Read_Thread_Finish);
		//time'd out, go to idle state
		OutputDebugString("Time'd out from wait state, calling initialize_read()\n");
		return false;
	}
	else if (response[0] == ENQ || response[0] == DC2)
	{
		ResetEvent(Ev_Send_Thread_Finish);
		OutputDebugString("Got an ENQ or DC2 from the wait state\n");
		return true;
	}
	return false;
}