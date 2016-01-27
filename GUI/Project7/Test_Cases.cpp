#include "Test_Cases.h"
VOID Test_Send_Enq()
{
	char str[1] = { ENQ };
	OutputDebugString("sending an ENQ\n");
	Send(str, sizeof(str), hWrite_Lock);
	DWORD total_bytes_read;
	char *response;
	if (!Wait_For_Data(&response, 1, 1000))
	{
		//Handle Timeout
	}else
	if (response[0] == ACK)
	{
		OutputDebugString("Got ACK Backk\n");
		Test_Send_Packet();
	}else
	if (response[0] == DC1)
	{
		OutputDebugString("Got DC Backk\n");
		Test_Send_Packet();
	}
	else
		OutputDebugString("not enq or dc.....its black magic\n");
}

VOID Test_Send_Packet()
{
	OutputDebugString("Sending packet!\n");
	char str[10] = { SOH, SYN0, DC1, DC1, 'a', 'b', 'c', 'd', 'e', EOT };
	for (int i = 0; i < strlen(str); i++)
	{
		char tmp[1] = { str[i] };
		Send(tmp, 1, hWrite_Lock);
	}
	//Send(str, strlen(str), hWrite_Lock);
	//DWORD total_bytes_read;
	char *response = "";
	if (!Wait_For_Data(&response, 1, 1000))
	{
		//Handle Timeout
	}else if (*response == ACK)
	{
		OutputDebugString("Got Ack Back!\n");
		OutputDebugString("Packet Sent successfully\n");
	}else if (response[0] == DC1)
	{
		OutputDebugString("Packet Sent successfully, Got DC Backk\n");
	}
	else
		OutputDebugString("aaaaaaaaaaaaaaa\n");
}

VOID Test_Transfer_Packet() {
	OutputDebugString("Sending packet v2!\n");
	char str[10] = { SOH, SYN0, DC1, DC1, 'a', 'b', 'c', 'd', 'e', EOT };
	Setup_Packet();
	// TODO: needs to loop through vector and pass packet into function
	//for (auto i : Write_Packets) {
		Transfer_Packet(str);
	//}
}

VOID Test_Checksum_Same_Packets() {
	//dummy packets
	char file [10] = { 'A', 0x1a, 'c', '?', 'E', 'F', '(', 'h', 'i', '\n' };
	char file2[10] = { 'A', 0x1a, 'c', '?', 'E', 'F', '(', 'h', 'i', '\n' };

	checksum *cs = new checksum();

	//add the characters to create a checksum
	for (char a : file) {
		cs->add(a);
	}
	std::vector<char> k = cs->get();

	cs->clear();
	for (char a : file2) {
		cs->add(a);
	}

	bool ok = cs->check(k[0], k[1]);

	if (ok) {
		OutputDebugString("true");
	}
	else {
		OutputDebugString("false");
	}
}

VOID Test_Checksum_Diff_Packets() {
	//dummy packets
	char file [10] = { SOH, SYN0, DC1, DC1, 'a', 'b', 'c', 'd', 'e', EOT };
	char file2[10] = { SOH, SYN1, DC1, DC1, 'a', 'b', 'e', 'd', 'c', EOT };

	checksum *cs = new checksum();

	//add the characters to create a checksum
	for (char a : file) {
		cs->add(a);
	}
	std::vector<char> k = cs->get();

	cs->clear();
	for (char a : file2) {
		cs->add(a);
	}

	bool ok = cs->check(k[0], k[1]);

	if (ok) {
		OutputDebugString("true");
	}
	else {
		OutputDebugString("false");
	}
}

/* TEST FUNCTIONS ONLY */
VOID Test_Initialize_Write(LPBYTE packet)
{

	Error_Check((wThread = CreateThread(NULL, 0, Test_Transfer_Packet, (LPVOID)packet, 0, &wThreadId)) == NULL ?
		ERR_WRITE_THREAD : NO_ERR);
}

VOID Test_Initialize_Write2(char* packet)
{
	WaitForSingleObject(Ev_Read_Thread_Finish, INFINITE);
	Error_Check((wThread = CreateThread(NULL, 0, Transfer_Packet, (LPVOID)packet, 0, &wThreadId)) == NULL ?
		ERR_WRITE_THREAD : NO_ERR);
}


DWORD WINAPI Test_Transfer_Packet(LPVOID packet)
{
	SetCommMask(hComm, RETURN_COMM_EVENT);
	WaitForSingleObject(hWrite_Lock, INFINITE);
	Test_Send_Enq();
	ReleaseMutex(hWrite_Lock);
	Initialize_Read();
	return 0;
}