#include "Datalink.h"

size_t fileSize;
std::string tempString = "";

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Setup_Packet
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Oscar Kwan
--
-- PROGRAMMER: Oscar Kwan
--
-- INTERFACE:  Setup_Packet()
--
-- RETURNS: std::vector<std::string> - a vector of packetized strings 
--
-- NOTES:
--		Takes a single text string (representing a text file) and splits it up into individual packets, based on the
--  defined packet size.  SOH and the alternating sync byte are inserted at the beginning of each packet, followed by
--  the 2 bytes of checksum calculated from the data in that packet.  If the remaining file data to be packetized is
--  greater than 512, we do not insert an EOT into the end of the packet.  If the remaining data is less than 512, we
--  insert an EOT after all the data bytes, resulting in a packet size that is less than 516.  All the packetized 
--  strings are put into the Write_Packets vector, where they will be sent individually to the write thread.
----------------------------------------------------------------------------------------------------------------------*/
std::vector<std::string> Setup_Packet() {
	size_t remainingData = fileSize;
	BOOL successPacket = false;
	BOOL successAllPacket = false;
	int count = 4;
	int j = 0;
	int sum;

	while (!successAllPacket) {

		//reset checksum to 0
		sum = 0;

		char *packet = new char[517];
		*packet = 0;
		// initialize packet header
		packet[0] = SOH;
		packet[1] = FlipSyncByte();

		// initialize checksum to 0 first, as these will also be added as part of the checksum
		packet[2] = DC1;
		packet[3] = DC2;

		// check for last packet
		if (remainingData < 512) {
			for (size_t i = tempString.length() - remainingData; i < tempString.length(); i++) {
				packet[count++] = tempString[i];
				remainingData--;
				// once no more data, put in EOT
				if (remainingData == 1) {
					// put last to EOT
					packet[count] = EOT;

					// calculate checksum and add it to the packet
					//for (char c : packet) {
					//	sum += c;
					//}

					//packet[2] = (char)(sum & 0x0000FF00);
					//packet[3] = (char)(sum & 0x000000FF);

					Write_Packets.push_back(packet);
					successAllPacket = true;
					break;
				}
			}
		}
		else {
			// loop through string character by character
			for (size_t i = j; i < tempString.length(); i++) {
				// increment count positon and assign character to each position in packet starting from 4
				packet[count++] = tempString[i];
				remainingData--;
				// once count is at 516, that means it is full, break out of loop
				if (count == 516) {
					j += 512;
					successPacket = true;
					break;
				}
			}
		}

		if (successPacket) {
			// set last char to null, this helps officially terminate the char array
			// if we don't we will get garbage values at the end (10 characters)
			// TODO: can keep looking into better implementation, right now its actually sending 511 chars of data with 1 null temrinator (\0)
			packet[516] = '\0';

			// resets packet data counter
			count = 4;

			// calculate the checksum and add it to the packet
			//for (char c : packet) {
			//	sum += c;
			//}

			//packet[2] = (char)(sum & 0x0000FF00);
			//packet[3] = (char)(sum & 0x000000FF);

			// stores into vector of packets
			Write_Packets.push_back(packet);

			// reset packet successful to false
			successPacket = false;
		}
	}

	return Write_Packets;
}