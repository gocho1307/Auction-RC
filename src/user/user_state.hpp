#ifndef __USER_STATE_HPP__
#define __USER_STATE_HPP__

#include "../lib/constants.hpp"
#include "../lib/protocol.hpp"

class UserState {
  public:
    std::string host = DEFAULT_AS_HOST;
    std::string port = DEFAULT_AS_PORT;

    // free with freeaddrinfo(addr);
    struct addrinfo *addrUDP = NULL;
    struct addrinfo *addrTCP = NULL;
    int socketUDP = -1;
    int socketTCP = -1; // current, if any

    bool shutDown = false;

    std::string line;

    bool loggedIn = false;
    std::string UID;
    std::string password;

    void readOpts(int argc, char *argv[]);
    void getServerAddresses();
    int openTCPSocket();
    int closeTCPSocket();
    int sendAndReceiveUDPPacket(Packet &packetOut, Packet &packetIn);
    int sendAndReceiveTCPPacket(Packet &packetOut, Packet &packetIn);
    ~UserState();
};

#endif // __USER_STATE_HPP__
