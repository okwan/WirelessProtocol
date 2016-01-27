#ifndef PHYSICAL_Read_H
#define PHYSICAL_Read_H

#include "Global.h"

// Create comm handle to serial port
VOID Initialize_Serial_Port();

// Create handle to thread
VOID Initialize_Read();

// Thread Implementation
DWORD WINAPI Read_Idle(LPVOID lpvoid);

VOID Send_Response();

// wait for a input. Using waitforcomm
LPSTR Wait_For_Input();

// Evaluate if the sender side has priority or not
BOOL Evaluate_Input(CHAR c);

// Wait for data state
VOID Wait_For_Packet();

// Validate the packet
BOOL Check_Packet(const char *packet);
#endif

