#ifndef OPENFILE_H
#define OPENFILE_H
#define MAX_FILE	1000000

#include "Global.h"
#include <commctrl.h>
#include <iostream>

extern HWND readFrom;
extern HWND sendTo;






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
void checkBoxChecked();

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
void initFileOpener();

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
void setFileOpenerFlags(int browserAction);

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
DWORD WINAPI createFileReader(LPVOID lpParam);

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
DWORD WINAPI createFileWriter(LPVOID lpParam);

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
void loadFileToView(const HWND *box, LPSTR file);

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
void addLine(const HWND *box, std::string line);

/*------------------------------------------------------------------------------------------------------------------
--	FUNCTION:		Packetize_Data
--
--	DATE:			December 24th, 2015
--
--	DESIGNER:		Jaegar Sarauer
--
--	REVISIONS:		Jaegar Sarauer	- Version 1 = Was orgionally for testing.
--					Oscar Kwan		- Version 2 = Rewritten for packetizing.
--
--	PROGRAMMER:		Jaegar Sarauer		Oscar Kwan
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
std::vector<std::string> Packetize_Data();

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
std::string getLine(const HWND *box, int line);

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
void setupProgressBar(const HWND *box);

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
void updateProgressBar(int currentPos);

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
int getLines(const HWND *box);

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
void updateStats(int structStats, int label);

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
void saveFileToComputer(const HWND *box, LPSTR file);

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
void clearBox(const HWND *box);

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
void clearStats();

#endif
