#include "server_state.hpp"
#include "../lib/messages.hpp"
#include "server.hpp"

#include <cstring>
#include <iostream>
#include <sys/socket.h>
#include <unistd.h>

void ServerState::readOpts(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:vh")) != -1) {
        switch (opt) {
        case 'p':
            this->port = std::string(optarg);
            break;
        case 'v':
            this->cverbose.active = true;
            break;
        case 'h':
            printHelp(std::cout, argv[0]);
            exit(EXIT_SUCCESS);
        default:
            printHelp(std::cerr, argv[0]);
            exit(EXIT_FAILURE);
        }
    }
}

void ServerState::openUDPSocket() {
    if ((this->socketUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    struct timeval UDPTimeout;
    memset(&UDPTimeout, 0, sizeof(UDPTimeout));
    UDPTimeout.tv_sec = SERVER_READ_TIMEOUT_SECS;
    if (setsockopt(this->socketUDP, SOL_SOCKET, SO_RCVTIMEO, &UDPTimeout,
                   sizeof(UDPTimeout)) < 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ServerState::openTCPSocket() {
    if ((this->socketTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    const int flag = 1;
    if (setsockopt(this->socketTCP, SOL_SOCKET, SO_REUSEADDR, &flag,
                   sizeof(int)) < 0) {
        std::cerr << SOCKET_REUSE_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void ServerState::getServerAddresses() {
    struct addrinfo hints;
    int res;

    // Get UDP address
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;    // Listen on 0.0.0.0
    if ((res = getaddrinfo(NULL, port.c_str(), &hints, &this->addrUDP)) != 0) {
        std::cerr << GETADDRINFO_UDP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (bind(this->socketUDP, this->addrUDP->ai_addr,
             this->addrUDP->ai_addrlen) == -1) {
        std::cerr << UDP_BIND_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get TCP address
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    hints.ai_flags = AI_PASSIVE;     // Listen on 0.0.0.0
    if ((res = getaddrinfo(NULL, port.c_str(), &hints, &this->addrTCP)) != 0) {
        std::cerr << GETADDRINFO_TCP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }
    if (bind(this->socketTCP, this->addrTCP->ai_addr,
             this->addrTCP->ai_addrlen) == -1) {
        std::cerr << TCP_BIND_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "[INFO] Starting to listen for connections on port: " << port
              << std::endl;
}

ServerState::~ServerState() {
    if (this->socketUDP != -1) {
        close(this->socketUDP);
    }
    if (this->socketTCP != -1) {
        close(this->socketTCP);
    }
    if (this->addrUDP != NULL) {
        freeaddrinfo(this->addrUDP);
    }
    if (this->addrTCP != NULL) {
        freeaddrinfo(this->addrTCP);
    }
}
