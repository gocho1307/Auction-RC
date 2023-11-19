#ifndef __USER_PROTOCOL_HPP__
#define __USER_PROTOCOL_HPP__

#include "../lib/protocol.hpp"

void sendAndReceiveUDPPacket(UDPPacket &packetOut, UDPPacket &packetIn);

void sendAndReceiveTCPPacket(TCPPacket &packetOut, TCPPacket &packetIn);

#endif // __USER_PROTOCOL_HPP__
