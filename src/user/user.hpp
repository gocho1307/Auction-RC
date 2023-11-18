#ifndef __USER_HPP__
#define __USER_HPP__

#include "../lib/constants.hpp"

#include <string>

class UserState {
  public:
    char *programPath;
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
    int UID = 0;
    std::string password;

    void readOpts(int argc, char *argv[]);
    void getServerAddresses();
    void openTCPSocket();
    void closeTCPSocket();
    void printTitle();
    void printHelp(std::ostream &stream);
    ~UserState();
};

void shutDownSigHandler(int sig);

#endif // __USER_HPP__
