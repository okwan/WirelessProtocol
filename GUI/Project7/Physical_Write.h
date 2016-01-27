#ifndef PHYSICAL_WRITE_H
#define PHYSICAL_WRITE_H
#include "Global.h"
/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Start_Packet_Loader_Thread
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  DWORD WINAPI Start_Packet_Loader_Thread(LPVOID lpvoid)
--
-- RETURNS: DWORD thread exit code
--
-- NOTES:
--		Packetize file opened on the GUI and feeds each packets into the write engine. Since we have to wait until
--      each packets to finish sending before sending the next one, we have to allocate a thread for this procedure or
--	    else it will hang in the main thread.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Start_Packet_Loader_Thread(LPVOID lpvoid);

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Initialize_Write
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  Initialize_Write(LPBYTE packet)
--					-char* packet : the packet to be sent
--
-- RETURNS: VOID
--
-- NOTES:
--		Initializes the write operation and reports any errors if it occurs
----------------------------------------------------------------------------------------------------------------------*/
VOID Initialize_Write(char* packet);

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Transfer_Packet
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Ruoqi Jia
--
-- PROGRAMMER: Ruoqi Jia
--
-- INTERFACE:  Transfer_Packet(LPVOID packet)
--					-LPVOID packet : the packet to be sent
--
-- RETURNS: 0 upon successful thread finish
--
-- NOTES:
--		Represents the sending state of the wireless protocol design.  This thread is called whenever a user chooses a
--  file to send and presses the Send button.  We confirm the line to make sure the line is available.  If confirm line
--  times out, we bring the protocol back to the Wait state to wait for an ENQ, and re-initialize the read thread.  If
--  confirm line succeeds, we send our packet using SendPacket.  When SendPacket succeeds or times out, we do a check
--  on the priority states of the sender and receiver.  If we don't have priority and the sender has priority, we enter
--  the wait state again to wait for an ENQ, otherwise we will just go directly to the read idle state.  Finally, we
--  reinitialize the read thread and return from this write thread.
----------------------------------------------------------------------------------------------------------------------*/
DWORD WINAPI Transfer_Packet(LPVOID packet);

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: ConfirmLine
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  ConfirmLine()
--
-- RETURNS: TRUE if we were able to get an ACK for the ENQ/DC2 that we sent, or
--          FALSE if we timeout on getting an ACK, and we exceeded the maximum number of tries to confirm the line.
--
-- NOTES:
--		Sends an ENQ or DC2 (depending on whether we want priority) to the serial port, to ask to have the line so we
--  can send a packet.  If we get an ACK back, we have successfully confirmed the line, and we return true.  If we
--  timeout and don't get an ACK back, we try again, up to the maximum number of times specified.
----------------------------------------------------------------------------------------------------------------------*/
BOOL ConfirmLine();

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: SendPacket
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  SendPacket(char* str)
--					-char* str : the packet to be sent
--
-- RETURNS: VOID
--
-- NOTES:
--		Sends a packet to the serial port.  If we get an ACK back acknowledging the packet we just sent, the packet is
--  considered sent successfully, and we return.  If we don't get an ACK back, we assume something went wrong with the
--  transmission and we resend the same packet, up to the maximum specified tries.
----------------------------------------------------------------------------------------------------------------------*/
VOID SendPacket(char* str);


/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: FlipSyncByte
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  FlipSyncByte()
--
-- RETURNS: CHAR representing the previous sync byte
--
-- NOTES:
--		Flips the sync byte based on the sync byte in the packet that was previously received.
----------------------------------------------------------------------------------------------------------------------*/
CHAR FlipSyncByte();

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: Evaluate_Response
--
-- DATE: November 25, 2015
--
-- REVISIONS: (Date and Description)
--
-- DESIGNER: Alvin Man
--
-- PROGRAMMER: Alvin Man
--
-- INTERFACE:  Evaluate_Response(char c)
--
-- RETURNS: BOOL - TRUE if we either got an ACK or DC1
--               - FALSE if we don't get either of those
--
-- NOTES:
--		Checks to see if we either got an ACK or DC1 as a response, and updates priority based on which is received.
----------------------------------------------------------------------------------------------------------------------*/
BOOL Evaluate_Response(char c);

#endif