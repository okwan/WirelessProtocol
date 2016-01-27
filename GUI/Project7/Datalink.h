#ifndef DATALINK_H
#define DATALINK_H

#include "Global.h"

//check size of file, use GetFileSize

//PACKETIZE DATA:
//array of packets should be created locally, because we don't know the size of the array
//packet buffer should be created locally, because the packet size may be less than 516bytes

BOOL Acknowledge_Line();
//VOID Setup_Packet();
std::vector<std::string> Setup_Packet();

#endif
