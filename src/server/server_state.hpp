#ifndef __SERVER_STATE_HPP__
#define __SERVER_STATE_HPP__

#include "../lib/constants.hpp"
#include "../lib/protocol.hpp"

#include <iostream>
#include <netdb.h>
#include <string>

class VerboseStream {
  public:
    bool active = false;

    template <class T> VerboseStream &operator<<(T rhs) {
        if (active) {
            std::cout << rhs;
        }
        return *this;
    }
    VerboseStream &operator<<(std::ostream &(*pf)(std::ostream &)) {
        if (active) {
            pf(std::cout);
        }
        return *this;
    }
    VerboseStream &operator<<(std::ostream &(*pf)(std::ios &)) {
        if (active) {
            pf(std::cout);
        }
        return *this;
    }
    VerboseStream &operator<<(std::ostream &(*pf)(std::ios_base &)) {
        if (active) {
            pf(std::cout);
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
    int globalAID = 0;

    void readOpts(int argc, char *argv[]);
    void openUDPSocket();
    void openTCPSocket();
    void getServerAddresses();
    ~ServerState();
};

#endif // __SERVER_STATE_HPP__
