
#ifndef GLOBAL_H
#define GLOBAL_H
#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
//#pragma warning disable 4996
#include <algorithm>
#include <string>
#include <vector>
#include <fstream>
#include <windows.h>
#include <regex>

#include "resource.h"
#include "Utility.h"
#include "OpenFile.h"
#include "Packetizer.h"
#include "Physical.h"
#include "Physical_Read.h"
#include "Physical_Write.h"
#include "Datalink.h"
#include "Session.h"
#include "Application.h"
#include "Test_Cases.h"
#include "checksum.h"

// Error checking values, start at 500
#define NO_ERR				500
#define ERR_READ_THREAD		501
#define ERR_INIT_COMM		502
#define ERR_WRITE_THREAD	503
#define ERR_RETRIEVE_COMM	504
#define ERR_DISPLAY_COMM	505
#define ERR_SET_COMM		506	
#define ERR_COMMMASK		507

// Packet makeup values
#define SOH		0x01
#define EOT		0x04
#define ENQ		0x05
#define ACK		0x06
#define DC1		0x21
#define DC2		0x22
#define SYN0	0x30
#define SYN1	0x31

#define MAX_PACKET_SIZE 516
#define PACKET_HEADER_SIZE	5

// Timeouts in ms
#define GLOBAL_TIMEOUT 400
#define MINI_TIMEOUT   250
#define S1_TIMEOUT     300

// Timeout max tries
#define CONFIRM_LINE_MAX_TRIES 5
#define SEND_PACKET_MAX_TRIES 5

// Custom Events
#define EV_SOH		TEXT("SOH")
#define EV_ACK		TEXT("ACK")
#define EV_ENQ		TEXT("ENQ")
#define EV_EOT		TEXT("EOT")
#define EV_DC1		TEXT("DC1")
#define EV_DC2		TEXT("DC2")
#define EV_OVREAD	TEXT("OVREAD")
#define EV_OVWRITE	TEXT("OVWRITE")
#define READ_LOCK	TEXT("READLOCK")
#define WRITE_LOCK	TEXT("WRITELOCK")

#define RETURN_COMM_EVENT	101

static	LPCSTR	lpszCommName	= "com1";	//Port name
static	TCHAR	Name[]			= TEXT("Comm Shell");
static	std::vector<std::string> Write_Packets;
static	std::vector<std::string> Read_Packets;
static	CHAR prev_sync_received = NULL;
static  CHAR prev_sync_sent = SYN1;

/*-------------------------------
*	File browser window variables
-------------------------------*/
#define SAVE_BROWSER	2530
#define OPEN_BROWSER	2531


/*-------------------------------
*	OpenFile.h layer variables
--------------------------------*/
extern HANDLE		hf;              // file handle
extern OPENFILENAME ofn;			 // common dialog box structure
extern HWND			hDlg;
extern char			szFile[260];     // buffer for file name
extern int			progressSize;

/*-------------------------------
*	Datalink.h layer variables
--------------------------------*/
extern HWND		hwnd;              // owner window
extern HANDLE	fileReadWriteThread;
extern DWORD	fileReadWriteID;
extern MSG		Msg;
extern std::string tempString;
extern size_t fileSize;


/*-------------------------------
*	Physical.h layer variables
--------------------------------*/
extern HANDLE	hComm;
static HANDLE	rThread; 
static DWORD	rThreadId;
static HANDLE	wThread;	// for packet sending thread
static DWORD	wThreadId;
static HANDLE	lThread;	// for file loader thread
static DWORD	lThreadId;	


// Since serial port is half-duplex, we need some mechanism to lock the read&write thread
extern HANDLE hWrite_Lock;
extern HANDLE hRead_Lock;




// For Physical_Read
extern HANDLE Ev_Send_Thread_Finish;
extern HANDLE Ev_Read_Thread_Finish;

extern BOOL receivedENQinWait;

/*-------------------------------
*	Priority.h layer variables
--------------------------------*/
extern BOOL senderHasPriority;
extern BOOL weHavePriority;

struct FILE_STATISTICS {
	int packetSent;
	int packetReceived;
	int packetLost;
	int packetCorruptedReceived;
	int acksReceived;
	int bitErrorRate;
};

extern FILE_STATISTICS stats;


























#endif


