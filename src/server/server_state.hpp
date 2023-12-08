#ifndef __SERVER_STATE_HPP__
#define __SERVER_STATE_HPP__

#include <iostream>
#include <map>
#include <netdb.h>
#include <string>
#include <sys/socket.h>

#include "../lib/constants.hpp"
#include "../lib/protocol.hpp"

class VerboseStream {
  public:
    bool active = false;

    template <class T> VerboseStream &operator<<(T val) {
        if (active) {
            std::cout << val;
        }
        return *this;
    }
    VerboseStream &operator<<(std::ostream &(*f)(std::ostream &)) {
        if (active) {
            f(std::cout);
        }
        return *this;
    }
};

class ServerState {
  public:
    std::string port = DEFAULT_AS_PORT;
    VerboseStream cverbose;

    // free with freeaddrinfo(addr);
    struct addrinfo *addrUDP = NULL;
    struct addrinfo *addrTCP = NULL;
    int socketUDP = -1;
    int socketTCP = -1; // current, if any

    bool shutDown = false;

    void readOpts(int argc, char *argv[]);
    void openUDPSocket();
    void openTCPSocket();
    void getServerAddresses();
    ~ServerState();
};

#endif // __SERVER_STATE_HPP__
