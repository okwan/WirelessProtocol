#define STRICT
#include "Application.h"


HWND hwnd;              // owner window
MSG Msg;
HANDLE	fileReadWriteThread;
DWORD	fileReadWriteID;


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
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hprevInstance,
	LPSTR lspszCmdParam, int nCmdShow) {

	// State - Build Window
	hDlg = CreateDialogParam(hInst, MAKEINTRESOURCE(IDD_DIALOG1), 0, WndProc, 0);
	ShowWindow(hDlg, nCmdShow);
	readFrom = GetDlgItem(hDlg, IDC_WRITERFILE);
	sendTo = GetDlgItem(hDlg, IDC_READERFILE);

	initFileOpener();

	SendMessage(GetDlgItem(hDlg, IDC_READERFILE), EM_SETLIMITTEXT, (LPARAM)MAX_FILE, NULL);
	SendMessage(GetDlgItem(hDlg, IDC_WRITERFILE), EM_SETLIMITTEXT, (LPARAM)MAX_FILE, NULL);

	Initialize_Serial_Port();

	// State - Enter Comm param 
	Setup_Comm_Config();

	// State - Engine Read Thread Start
	Initialize_Read();

	while (GetMessage(&Msg, NULL, 0, 0)) {
		TranslateMessage(&Msg);
		DispatchMessage(&Msg);
	}

	return Msg.wParam;
}


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
INT_PTR CALLBACK WndProc(HWND hDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_PRIORITYCHECK:
			switch (HIWORD(wParam)) {
			case BN_CLICKED:
				checkBoxChecked();
				break;
			}
			break;
		case IDC_BUTTONCOMMSETTINGS:
			Setup_Comm_Config();
			break;
		case IDC_BUTTONOPEN:
			setFileOpenerFlags(OPEN_BROWSER);
			if (GetOpenFileName(&ofn) == TRUE) {
				fileReadWriteThread = CreateThread(NULL, 0, createFileReader, NULL, 0, &fileReadWriteID);
			}
			loadFileToView(&readFrom, ofn.lpstrFile);
			break;
		case IDC_BUTTONSAVE:
			SendMessage(readFrom, EM_SETREADONLY, (LPARAM)TRUE, NULL);
			setFileOpenerFlags(SAVE_BROWSER);
			if (GetSaveFileName(&ofn) == TRUE) {
				fileReadWriteThread = CreateThread(NULL, 0, createFileWriter, NULL, 0, &fileReadWriteID);
			}
			saveFileToComputer(&readFrom, ofn.lpstrFile);
			SendMessage(readFrom, EM_SETREADONLY, (LPARAM)FALSE, NULL);
			break;
		case IDC_BUTTONSAVE2:
			setFileOpenerFlags(SAVE_BROWSER);
			if (GetSaveFileName(&ofn) == TRUE) {
				fileReadWriteThread = CreateThread(NULL, 0, createFileWriter, NULL, 0, &fileReadWriteID);
			}
			saveFileToComputer(&sendTo, ofn.lpstrFile);
			break;
		case IDC_BUTTONCLEAR:
			clearBox(&readFrom);
			break;
		case IDC_BUTTONCLEAR2:
			clearBox(&sendTo);
			break;
		case IDC_BUTTONCLEAR3:
			clearStats();
			break;
		case IDC_BUTTONSEND:
			setupProgressBar(&readFrom);
			Error_Check((wThread = CreateThread(NULL, 0, Start_Packet_Loader_Thread, NULL, 0, &wThreadId)) == NULL ?
				ERR_WRITE_THREAD : NO_ERR);
			break;
		}
		break;

	case WM_CLOSE:	// Terminate program
		PostQuitMessage(0);
		break;
	default:
		return false;
	}
	return 0;
}