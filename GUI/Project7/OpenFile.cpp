#include "OpenFile.h"


//file opener vars
//file opener dialog structure.
OPENFILENAME ofn;

//handle of the file opener.
HANDLE hf;

//handle of the main GUI window.
HWND hDlg;

//left side edit box handle
HWND readFrom;

//right side edit box handle.
HWND sendTo;

//char* for file path.
char szFile[260];

//the amount of lines in the writer window (the left text window).
int writerFileLines;

//the amount of lines in the reader window (the right text box).
int readerFileLines;

std::regex addNewLine("(?!\r)\n");

//the max size of the progress bar.
int progressSize;










/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		checkBoxChecked
--
--	DATE:			November 30th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void checkBoxChecked();
--
--	RETURNS:		VOID
--
--	NOTES: 			This function updates the priority from the check
--					box on the GUI, next to the "Send" button. It is intended
--					to be called when the user clicks on the checkbox,
--					allowing the status to be updated.
-------------------------------------------------------------------------------------------------------------------*/
void checkBoxChecked() {
	weHavePriority = SendMessage(GetDlgItem(hDlg, IDC_PRIORITYCHECK), BM_GETCHECK, 0, 0);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		createFileReader
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		DWORD WINAPI createFileReader(LPVOID lpParam);
--						LPVOID lpParam = Required by the CreateThread function to be able to multithread this action.
--
--	RETURNS:		DWORD WINAPI
--
--	NOTES: 			This function creates a file on the handle for the file opener. The file opener is a window which allows
--					the user to browse through their computer. This function is specific to opening the file browser to open
--					files, rather than save them.
--
--					Because there is only one window that needs to deal with the loading of files, this is specifically designed
--					for the left side, "local" window.
--
--					The window is built on the OPENFILENAME struct, which details information about constraints and features
--					the window will offer the user. More details on the OPENFILENAME struct and how it is setup is available
--					at the description for initFileOpener function.
-------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI createFileReader(LPVOID lpParam) {
	hf = CreateFile(ofn.lpstrFile,
		GENERIC_READ,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)hDlg);
	return 0;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		createFileWriter
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		DWORD WINAPI createFileReader(LPVOID lpParam);
--						LPVOID lpParam = Required by the CreateThread function to be able to multithread this action.
--
--	RETURNS:		DWORD WINAPI
--
--	NOTES:			This function creates a file on the handle for the file opener. The file opener is a window which
--					allows the user to browse through their computer. This function is specific to opening the file
--					browser to save	files, rather than open them.
--
--					Both windows of this application require the need to use the save window.
--
--					The window is built on the OPENFILENAME struct, which details information about constraints and features
--					the window will offer the user. More details on the OPENFILENAME struct and how it is setup is available
--					at the description for initFileOpener function.
-------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI createFileWriter(LPVOID lpParam) {
	hf = CreateFile(ofn.lpstrFile,
		GENERIC_WRITE,
		0,
		(LPSECURITY_ATTRIBUTES)NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		(HANDLE)hDlg);
	return 0;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		loadFileToView
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void loadFileToView(const HWND *box, LPSTR file);
--						const HWND *box		= The specific HWND this file is being loaded to.
--						LPSTR file			= The file path the file will be read from.
--
--	RETURNS:		VOID
--
--	NOTES:			This function uses the constructed file from the createFileReader to load a plain text file
--					into the specified handle, (*box).
--
--					The function parses the file line by line, and sends each parsed line as a line into the specified
--					handle (normally an Edit Box). In order for this to populate the correctly, the function must find
--					the end of the file each time it appends a line to the handle, to prevent file overwrites.
-------------------------------------------------------------------------------------------------------------------*/
void loadFileToView(const HWND *box, LPSTR file) {
	int idx;
	std::string tmp;
	std::ifstream inputF(file);
	clearBox(box);
	SendMessage(*box, EM_SETREADONLY, (LPARAM)FALSE, NULL);
	while (getline(inputF, tmp)) {
		tmp += "\n";
		idx = GetWindowTextLength(*box);
		SendMessage(*box, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
		SendMessage(*box, EM_REPLACESEL, 0, (LPARAM)(tmp.c_str()));
	}
	writerFileLines = SendMessage(*box, EM_GETLINECOUNT, NULL, NULL);

	setupProgressBar(&readFrom);
	inputF.close();
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		clearBox
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void clearBox(const HWND *box);
--						const HWND *box		= The specific HWND that the text is being set to empty.
--
--	RETURNS:		VOID
--
--	NOTES:			This function takes in a handle to a UI element, and sets it's text to be an empty string. It
--					is inteded to be called when the user clicks on one of the clear buttons for the Edit Boxes.
-------------------------------------------------------------------------------------------------------------------*/
void clearBox(const HWND *box) {
	SetWindowText(*box, "");
}


/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		saveFileToComputer
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void saveFileToComputer(const HWND *box, LPSTR file);
--						const HWND *box		= The HWND to target to save the data from.
--						LPSTR file			= The file path to save a new file to.
--
--	RETURNS:		VOID
--
--	NOTES:			This function requires to know which Edit Control it will be getting its data from. When this
--					function is called, the file to write to is opened and line-by-line, the data is sent from the
--					specified edit control into the new, or previously created file (overwriting previous data).
--
-------------------------------------------------------------------------------------------------------------------*/
//loads a file (from a file path) into the left side window, line by line.
void saveFileToComputer(const HWND *box, LPSTR file) {
	int idx;
	size_t onLine = 0,
		totalLines = getLines(box);
	std::string tmp;
	std::ofstream outputF(file);
	for (; onLine < totalLines; onLine++) {
		outputF << getLine(box, onLine);
	}
	outputF.close();

}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		getLines
--
--	DATE:			December 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		int getLines(const HWND *box)
--						const HWND *box		=
--
--	RETURNS:		int		=		Returns the amount of lines of text the specific UI element has.
--
--	NOTES:			This function targets the specified Edit Control, and returns the amount of lines there are of text
--					in the control.
--
-------------------------------------------------------------------------------------------------------------------*/
//returns the amount of lines in the specific handle.
int getLines(const HWND *box) {
	return SendMessage(*box, EM_GETLINECOUNT, NULL, NULL);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		Packetize_Data
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		std::vector<std::string> Packetize_Data()
--
--	RETURNS:		std::vector<std::string>	= A vector of strings, each of size 512 characters, or less if not
--												  available.
--
--	NOTES:			This function parses all the data in the Writer File Edit Box (the left Edit Control), and puts
--					the strings into a vector, which will be returned to the Physical_Write file for processing to send.
--
-------------------------------------------------------------------------------------------------------------------*/
/*
This function is for UI testing.
It read all the lines in IDC_WRITERFILE and send them in fake packets sizes of
one line (not set size).
This function is just for testing, once packet reading is done, this is a useless function.
*/
std::vector<std::string> Packetize_Data() {
	size_t i;
	//handle of box were sending to.
	SendMessage(GetDlgItem(hDlg, IDC_WRITERFILE), EM_SETREADONLY, (LPARAM)TRUE, NULL);

	for (i = 0; i < writerFileLines; i++) {
		//addLine(&sendTo, getLine(&readFrom, i));
		tempString += getLine(&readFrom, i);
		updateProgressBar(i);
	}

	fileSize = tempString.length();

	std::vector<std::string> tempWritePackets = Setup_Packet();
	return tempWritePackets;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		setupProgressBar
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void setupProgressBar(const HWND *box)
--						const HWND *box		=
--
--	RETURNS:		VOID
--
--	NOTES:			This function sets up the maximum number for the progress bar, which is the amount of characters
--					in the file to send. The inention of this number, is to incrememnt the bar as characters are
--					successufully sent to its reciever.
--
-------------------------------------------------------------------------------------------------------------------*/
//this sets the size of the progress bar. It is expected that you would pass in the number of lines of the file.
void setupProgressBar(const HWND *box) {
	progressSize = GetWindowTextLength(*box);
	SendMessage(GetDlgItem(hDlg, IDC_FILEPROGRESS), PBM_SETRANGE32, 0, progressSize);
	SendMessage(GetDlgItem(hDlg, IDC_FILEPROGRESS), PBM_SETPOS, 0, 0);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		updateProgressBar
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void updateProgressBar(int currentPos);
--						int currentPos		= How much to increment the file progress bar by.
--
--	RETURNS:		VOID
--
--	NOTES:			This function updates the file progress bar, by incrementing it by a number. (It increases, no need
--					to find out what value it sits at and add to it.)
-------------------------------------------------------------------------------------------------------------------*/
//this sets the current position on the progress bar. 
void updateProgressBar(int currentPos) {
	SendMessage(GetDlgItem(hDlg, IDC_FILEPROGRESS), PBM_SETPOS, SendMessage(GetDlgItem(hDlg, IDC_FILEPROGRESS), PBM_GETPOS, 0, 0) + currentPos, 0);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		addLine
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1 = Default sending lines to control, tested with the two Edit controls.
--					Jaegar Sarauer - Version 2 = Added regex parsing to insert new lines (requires \r\n rather than
--												 just \n character.
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void addLine(const HWND *box, std::string line)
--						const HWND *box		=
--						std::string line	=
--
--	RETURNS:		VOID
--
--	NOTES:			This function adds a single string to the end of the specified edit control. This line will be
--					ended with a new line character, requiring that you will send what you expect to be your entire
--					line at once. (You may not parse to a previously added line.)
--
-------------------------------------------------------------------------------------------------------------------*/
//adds a line to whatever handle you would like. It is expected to pass in a file holder as the handle.
void addLine(const HWND *box, std::string line) {
	int idx;
	idx = GetWindowTextLength(*box);
	std::regex e("(?!\r)\n");
	std::string tmp = std::regex_replace(line, e, "\r\n");
	SendMessage(*box, EM_SETSEL, (LPARAM)idx, (LPARAM)idx);
	SendMessage(*box, EM_REPLACESEL, 0, (LPARAM)tmp.c_str());
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		getLine
--
--	DATE:			 , 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		std::string getLine(const HWND *box, int line)
--						const HWND *box		= The HWND to which Edit Box the line is being retrieved from.
--						int line			= The line index which is being retrived from the specified edit box.
--
--	RETURNS:		std::string				= The string retrieved from the specified line.
--
--	NOTES:			This function gets a single line, specified from the int line parameter, and returns it.
--
-------------------------------------------------------------------------------------------------------------------*/
//this function targets a handle and gets the line as string at an index you choose.
std::string getLine(const HWND *box, int line) {
	size_t len;
	TCHAR *tmp;
	len = SendMessage(*box, EM_LINELENGTH, SendMessage(*box, EM_LINEINDEX, line, 0), NULL);
	tmp = new TCHAR[len];
	*tmp = 0;
	SendMessage(GetDlgItem(hDlg, IDC_WRITERFILE), EM_GETLINE, line, (LPARAM)tmp);
	tmp[len] = '\0';
	return tmp;
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		initFileOpener
--
--	DATE:			November 23rd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void initFileOpener();
--
--	RETURNS:		VOID
--
--	NOTES:			This function initializes the default paramters for the file opener. This function only needs
--					to be called once to set it up. At the end of the function, setFileOpenerFlags is called to setup
--					default flags, however these would be reassigned each time a file opener for save or open is called.
--
-------------------------------------------------------------------------------------------------------------------*/
//initializes the parameters for the file opener. This only needs to be called once to set it up.
void initFileOpener() {
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hDlg;
	ofn.lpstrFile = szFile;
	// Set lpstrFile[0] to '\0' so that GetOpenFileName does not 
	// use the contents of szFile to initialize itself.
	ofn.lpstrFile[0] = '\0';
	ofn.nMaxFile = sizeof(szFile);
	ofn.lpstrFilter = "Text Files\0*.txt\0";
	ofn.lpstrDefExt = "txt";
	ofn.nFilterIndex = 1;
	ofn.lpstrFileTitle = NULL;
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = NULL;
	setFileOpenerFlags(OPEN_BROWSER);
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		setFileOpenerFlags
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1 = The origional code was used in the initFileOpener.
--					Jaegar Sarauer - Version 2 = The code was split into this function so the flags may be changed, and
--												 still use the same struct for the file opener.
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void setFileOpenerFlags(int browserAction)
--						int browserAction		= Is the file browser being set for reading or writing.
--
--	RETURNS:		VOID
--
--	NOTES:			This function sets the correct flags for what type of file processing it is doing. (reading or
--					saving a file).
--
-------------------------------------------------------------------------------------------------------------------*/
void setFileOpenerFlags(int browserAction) {
	ofn.Flags = 0;
	switch (browserAction) {
	case SAVE_BROWSER:
		ofn.Flags = OFN_EXPLORER | OFN_PATHMUSTEXIST | OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT | OFN_ENABLESIZING;
		break;
	case OPEN_BROWSER:
		ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST | OFN_ENABLESIZING | OFN_HIDEREADONLY;
		break;
	}
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		updateStats
--
--	DATE:			December 1st, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void updateStats(int structStats, int label)
--						int structStats		= What are we setting the value to.
--						int label			= Which stat in the stat section are we setting?
--
--	RETURNS:		VOID
--
--	NOTES:			This function sets a number to a specific stat. The numbers must be kept elsewhere for updating.
--
-------------------------------------------------------------------------------------------------------------------*/
void updateStats(int structStats, int label) {
	std::string temp;
	temp = std::to_string(structStats);
	SendMessage(GetDlgItem(hDlg, label), WM_SETTEXT, NULL, (LPARAM)temp.c_str());
}



/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		clearStats
--
--	DATE:			December 2nd, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer - Version 1
--
--	PROGRAMMER:		Jaegar Sarauer
--
--	INTERFACE:		void clearStats()
--
--	RETURNS:		VOID
--
--	NOTES:			This functions clears all the stats on the GUI, in the Stats section. All the stats are sent back
--					to 0.
--
-------------------------------------------------------------------------------------------------------------------*/
void clearStats() {
	size_t label = 1038;
	for (; label <= 1044; label++)
		SendMessage(GetDlgItem(hDlg, label), WM_SETTEXT, NULL, (LPARAM)"0");
}