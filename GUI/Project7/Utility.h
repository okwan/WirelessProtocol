#ifndef UTILITY_H
#define UTILITY_H
#include "Global.h"

VOID Error_Check(DWORD err);
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
VOID Output_GetLastError();

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
VOID Setup_Comm_Config();
#endif