#ifndef __PACKETS_HPP__
#define __PACKETS_HPP__

#include "server_state.hpp"

#include <functional>
#include <map>

typedef std::map<std::string,
                 std::function<void(ServerState &, std::string, Address)>>
    UDPPacketsHandler;
typedef std::map<std::string, std::function<void(const int)>> TCPPacketsHandler;

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom);
void interpretTCPPacket(const int fd);

// UDP
void LINHandler(ServerState &state, std::string msg, Address UDPFrom);
void LOUHandler(ServerState &state, std::string msg, Address UDPFrom);
void UNRHandler(ServerState &state, std::string msg, Address UDPFrom);
void LMAHandler(ServerState &state, std::string msg, Address UDPFrom);
void LMBHandler(ServerState &state, std::string msg, Address UDPFrom);
void LSTHandler(ServerState &state, std::string msg, Address UDPFrom);
void SRCHandler(ServerState &state, std::string msg, Address UDPFrom);

// TCP
void OPAHandler(const int fd);
void CLSHandler(const int fd);
void BIDHandler(const int fd);
void SASHandler(const int fd);

#endif // __PACKETS_HPP__
