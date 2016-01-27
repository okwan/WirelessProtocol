#ifndef TEST_CASES
#define TEST_CASES
#include "Global.h"
VOID Test_Send_Enq();
VOID Test_Send_Packet();
VOID Test_Transfer_Packet();
VOID Test_Checksum_Same_Packets();
VOID Test_Checksum_Diff_Packets();


VOID Test_Initialize_Write(LPBYTE packet);
DWORD WINAPI Test_Transfer_Packet(LPVOID packet);
VOID Test_Initialize_Write2(char* packet);
#endif