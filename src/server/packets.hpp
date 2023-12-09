#ifndef __PACKETS_HPP__
#define __PACKETS_HPP__

#include "server_state.hpp"
#include <functional>

typedef std::map<std::string,
                 std::function<int(ServerState &, std::string, Address)>>
    UDPPacketsHandler;
typedef std::map<std::string, std::function<int(ServerState &, const int)>>
    TCPPacketsHandler;

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom);
void interpretTCPPacket(ServerState &state, const int fd);

// UDP
int LINHandler(ServerState &state, std::string msg, Address UDPFrom);
int LOUHandler(ServerState &state, std::string msg, Address UDPFrom);
int UNRHandler(ServerState &state, std::string msg, Address UDPFrom);
int LMAHandler(ServerState &state, std::string msg, Address UDPFrom);
int LMBHandler(ServerState &state, std::string msg, Address UDPFrom);
int LSTHandler(ServerState &state, std::string msg, Address UDPFrom);
int SRCHandler(ServerState &state, std::string msg, Address UDPFrom);

// TCP
int OPAHandler(ServerState &state, const int fd);
int CLSHandler(ServerState &state, const int fd);
int BIDHandler(ServerState &state, const int fd);
int SASHandler(ServerState &state, const int fd);

#endif // __PACKETS_HPP__
