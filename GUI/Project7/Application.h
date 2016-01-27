#ifndef APPLICATION_H
#define APPLICATION_H

#include "Global.h"


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		WinMain
--
--	DATE:			November 30th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1		= Initial creation and UI design.
--
--	PROGRAMMER:		Jaegar Sarauer		Ruoqi Jia		Alvin Man		Oscar Kwan
--
--	INTERFACE:		int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance, LPSTR lspszCmdParam, int nCmdShow)
--						HINSTANCE hInst				= Parent handle to create UI thread on.
--						HINSTANCE hprevInstance		= Unused in this function, required by WINAPI to run main.
--						LPSTR lspszCmdParam			= Unused in this function, required by WINAPI to run main.
--						int nCmdShow				= Id of the main window that the program is based on.
--
--	RETURNS:		int WINAPI						= End of run result.
--
--	NOTES: 			This function sets up the base parameters and objects and initializes the main GUI and handles
--					to the Edit Controls (as they are accessed a lot). Edit control max sizes are declared here as
--					well, as well as the call to initialize the serial port.
--					The loop to read messages to the program is created at the end of this function. Allows the program
--					to continue running.
-------------------------------------------------------------------------------------------------------------------*/
INT_PTR CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		WndProc
--
--	DATE:			November 30th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Several Versions = Had to be constantly updated as new buttons/controls were added,
--														and the functions to go with them were changed.
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		INT_PTR CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
--						HWND hDlg			= The Main GUI handle.
--						UINT uMsg			= The message to read and process.
--						WPARAM wParam		= First generic parameter (may be null depending on the message).
--						LPARAM lParam		= Second generic parameter (may be null depending on the message).
--
--	RETURNS:		INT_PTR					= Result of the message process.
--
--	NOTES: 			This function handles all messages to do with the program. User controlled messages are processed
--					here to check for what should be done with the command. All user commands lead to functions which
--					will call their actions.
-------------------------------------------------------------------------------------------------------------------*/
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow);



#endif