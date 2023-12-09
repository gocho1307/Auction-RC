#include "user_state.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "user.hpp"

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

void UserState::readOpts(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "n:p:h")) != -1) {
        switch (opt) {
        case 'n':
            this->host = std::string(optarg);
            break;
        case 'p':
            this->port = std::string(optarg);
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

void UserState::openUDPSocket() {
    if ((this->socketUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = USER_READ_TIMEOUT_SECS;
    if (setsockopt(this->socketUDP, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) != 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << strerror(errno) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int UserState::openTCPSocket() {
    if ((this->socketTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << strerror(errno) << std::endl;
        return 1;
    }
    struct timeval timeout;
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = USER_READ_TIMEOUT_SECS;
    if (setsockopt(this->socketTCP, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) != 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << strerror(errno) << std::endl;
        return 1;
    }
    return 0;
}

int UserState::closeTCPSocket() {
    if (this->socketTCP == -1) {
        return 0; // socket was already closed
    }
    if (close(this->socketTCP) != 0) {
        std::cerr << SOCKET_CLOSE_ERR << strerror(errno) << std::endl;
        return 1;
    }
    this->socketTCP = -1;
    return 0;
}

void UserState::getServerAddresses() {
    struct addrinfo hints;
    int res;

    // Get UDP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    if ((res = getaddrinfo(host.c_str(), port.c_str(), &hints,
                           &this->addrUDP)) != 0) {
        std::cerr << GETADDRINFO_UDP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get TCP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if ((res = getaddrinfo(host.c_str(), port.c_str(), &hints,
                           &this->addrTCP)) != 0) {
        std::cerr << GETADDRINFO_TCP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }
}

int UserState::sendAndReceiveUDPPacket(UDPPacket &packetOut,
                                       UDPPacket &packetIn, size_t lim) {
    std::string response;
    if (sendUDPPacket(packetOut, this->addrUDP->ai_addr,
                      this->addrUDP->ai_addrlen, this->socketUDP)) {
        return 1;
    }
    if (receiveUDPPacket(response, this->addrUDP->ai_addr,
                         &this->addrUDP->ai_addrlen, this->socketUDP, lim)) {
        return 1;
    }
    if (packetIn.deserialize(response)) {
        std::cerr << PACKET_ERR << std::endl;
        return 1;
    }
    return 0;
}

int UserState::sendAndReceiveTCPPacket(TCPPacket &packetOut,
                                       TCPPacket &packetIn) {
    std::string response;
    if (this->openTCPSocket()) {
        return 1;
    }
    if (connect(this->socketTCP, this->addrTCP->ai_addr,
                this->addrTCP->ai_addrlen) == -1) {
        this->closeTCPSocket();
        std::cerr << TCP_CONNECT_ERR << std::endl;
        return 1;
    }
    if (packetOut.serialize(this->socketTCP)) {
        this->closeTCPSocket();
        return 1;
    }
    if (packetIn.deserialize(this->socketTCP)) {
        this->closeTCPSocket();
        std::cerr << PACKET_ERR << std::endl;
        return 1;
    }
    return this->closeTCPSocket();
}

UserState::~UserState() {
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
