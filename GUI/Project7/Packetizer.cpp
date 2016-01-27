#include "Packetizer.h"

/*
Unused class,
written by Jaeagr Sarauer.
Intended to be the packetizer, but went unused.
*/


//left side edit box handle
extern HWND RreadFrom = GetDlgItem (hDlg, IDC_WRITERFILE);

//right side edit box handle.
extern HWND RsendTo = GetDlgItem (hDlg, IDC_READERFILE);

	//this gets called when a packet is successfully sent.
	//it will pop the front packet off, and push on a new one.
	void Packetizer::updatePacketList () {
		if (packet_list.size() > 0)
			packet_list.erase (packet_list.begin ());
		if (packet_list.size () < PACKET_LIST_MAX)
			appendPackets ();
	}

	//this function appends packets to the end of the packet_list.
	void Packetizer::appendPackets () {
		size_t i;
		for (i = packet_list.size (); i < PACKET_LIST_MAX; i++)
			packet_list.push_back (buildPacket ());
	}

	//this function will add a single line from a edit box to the temp_buffer.
	void Packetizer::populateBuffer () {
		temp_buffer.append( getLine (&RreadFrom, currentLine++));
	}

	//checks to see if it can retrieve any more packets.
	//returns true if there are no more packets to make.
	bool Packetizer::outOfData () {
		// ----------- testing

		char t[512];
		_itoa_s (temp_buffer.length (), t, 10);
		OutputDebugString (t);
		// -------------------
		return (currentLine >= getLines (&RreadFrom) && temp_buffer.length() == 0);
	}

	//retrieves the first 512 characters from the temp_buffer and removes them from the buffer.
	std::string Packetizer::buildPacket () {
		std::string tmp;
		while (packet_list.size () < PACKET_LIST_MAX && temp_buffer.length() < BUFFER_CHAR_SIZE && currentLine < getLines (&RreadFrom)) {//&& outOfData ()) {
			populateBuffer ();
		}
		tmp = temp_buffer.substr(0, BUFFER_CHAR_SIZE);
		temp_buffer.erase (0, BUFFER_CHAR_SIZE);
		return tmp;
	}

	//returns the first packet im the packet_list. 
	//Returns null if no packets left.
	std::string Packetizer::getPacket () {
		return packet_list[0];//begin ();
	}