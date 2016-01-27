#ifndef PHYSICAL_H
#define PHYSICAL_H
#include "Global.h"

// Reads any kind of data from the serial port :)
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
--									LPDWORD total_bytes_read,
--									DWORD	TIMEOUT,
--									HANDLE	lock)
--					- char **str				: Buffer to store the content retrived by Read_File
--					- DWORD buffer_size			: The size of the buffer, has to be specified explicitly
--					- LPDWORD total_bytes_read	: total bytes actually read frmo the serial port,
--							might be smaller than 516
--					- DWORD TIMEOUT				: Waiting time
--					- HANDLE lock				: basically pass in hRead_Lock if you're operating in
--							Physical_Read.h, hWrite_Lock if in Physical_Write.h
--
-- RETURNS: TRUE if data read successfully, FALSE if timeout
--
-- NOTES:
--	Pretty much reads any kind of data from the serial port :)
----------------------------------------------------------------------------------------------------------------------*/
BOOL Wait_For_Data(char **str, DWORD buffer_size, DWORD TIMEOUT);

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
--	 Send any kind of data to the serial port
----------------------------------------------------------------------------------------------------------------------*/

VOID Send(char* msg, DWORD size, HANDLE lock);
VOID Send(BYTE command);
BOOL Timeout(DWORD msec);
BOOL WaitForEnq();
#endif