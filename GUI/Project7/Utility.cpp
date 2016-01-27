#include "Utility.h"

VOID Error_Check(DWORD err)
{
	LPCTSTR msg = NULL;
	switch (err)
	{
		case NO_ERR:
			return;
			break;
		case ERR_READ_THREAD :
			msg = "Failed to create read thread handle\n";
			break;
		case ERR_INIT_COMM :
			msg = "Failed to initialize serial port handle\n";
			break;
		case ERR_WRITE_THREAD :
			msg = "Failed to create write thread handle\n";
			break;
		case ERR_RETRIEVE_COMM :
			msg = "Failed to retrieve communication configuration\n";
			break;
		case ERR_DISPLAY_COMM :
			msg = "Failed to load configuration box\n";
			break;
		case ERR_SET_COMM:
			msg = "Failed to set communication parameters\n";
		case ERR_COMMMASK : 
			msg = "Failed to set comm mask";
			break;
	}
	MessageBox(NULL, msg, "Error", MB_OK); 
	OutputDebugString(msg);
	Output_GetLastError();		// Display formatted error message onto console output
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: OutPut_GetLastError
--
-- DATE: September 28, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL OutPut_GetLastError();
--
-- RETURNS: VOID
--
-- NOTES:
--	Outputs a formatted string from the GetLastError() function onto the console.
--	Mainly used for debugging
----------------------------------------------------------------------------------------------------------------------*/
VOID Output_GetLastError()
{
	wchar_t buf[256];			//Buffer to the formatted error string
	FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM, NULL, GetLastError(),
		MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), buf, 256, NULL);	//Format the DWORD into buffer
	OutputDebugStringW(buf);	//Output buffer 
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Setup_Comm_Config
--
-- DATE: November 20, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE: BOOL Setup_Comm_Config();
--
--
-- RETURNS: TRUE if the DCB struture is successfully set by the driver-supplied configuration dialog
--				FALSE otherwise
--
-- NOTES:
--	Opens a driver-supplied configuration dialog box for user to initialize the protocol values in a DCB structure
--	so two devices can communicate between each other
----------------------------------------------------------------------------------------------------------------------*/
VOID Setup_Comm_Config()
{
	COMMCONFIG cc;					//Configuration state of the serial port
	cc.dwSize = sizeof(COMMCONFIG);	//Set the size of the structure to default
	cc.wVersion = 0x100;			//Set the version number 
	Error_Check(!GetCommConfig(hComm, &cc, &cc.dwSize) ? ERR_RETRIEVE_COMM : NO_ERR);		//Retrieves the current configuration of the serial port
	Error_Check(!CommConfigDialog(lpszCommName, hwnd, &cc) ? ERR_DISPLAY_COMM : NO_ERR);		//Display configuration box 
	Error_Check(!SetCommState(hComm, &cc.dcb) ? ERR_SET_COMM : NO_ERR);						//Set cc to the current configuration for the serial port
}