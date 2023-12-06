#include "server_state.hpp"
#include "../lib/messages.hpp"
#include "packets_handler.hpp"
#include "server.hpp"

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <sys/socket.h>
#include <unistd.h>

void ServerState::readOpts(int argc, char *argv[]) {
    int opt;
    while ((opt = getopt(argc, argv, "p:v:h")) != -1) {
        switch (opt) {
        case 'p':
            this->port = std::string(optarg);
            break;
        case 'v':
            this->verbose_mode = true;
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

void ServerState::resolveServerAddress() {
    struct addrinfo hints;
    int res;
    const char *port_str = port.c_str();

    // Get UDP address
    memset(&hints, 0, sizeof hints);
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    hints.ai_flags = AI_PASSIVE;    // Listen on 0.0.0.0
    if ((res = getaddrinfo(NULL, port_str, &hints, &this->addrUDP)) != 0) {
        std::cerr << GETADDRINFO_UDP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }

    if (bind(this->socketUDP, this->addrUDP->ai_addr,
             this->addrUDP->ai_addrlen) == -1) {
        std::cerr << "Error binding UDP socket: ";
        perror(""); // This will print the error description
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
        std::cerr << TCP_BIND_ERR << std::endl;
        exit(EXIT_FAILURE);
    }

    std::cout << "Listening for connections on port " << port << std::endl;
}

void ServerState::registerPacketHandlers() {
    // UDP
    udp_packets_handler.insert({LINPacket::ID, handleLogin});
    udp_packets_handler.insert({LOUPacket::ID, handleLogout});
    udp_packets_handler.insert({UNRPacket::ID, handleUnregister});
    udp_packets_handler.insert({LMAPacket::ID, handleMyAuctions});
    udp_packets_handler.insert({LMBPacket::ID, handleMyBids});
    udp_packets_handler.insert({LSTPacket::ID, handleList});
    udp_packets_handler.insert({SRCPacket::ID, handleShowRecord});

    // TCP
    tcp_packets_handler.insert({OPAPacket::ID, handleOpenAuction});
    tcp_packets_handler.insert({CLSPacket::ID, handleCloseAuction});
    tcp_packets_handler.insert({BIDPacket::ID, handleBid});
    tcp_packets_handler.insert({SASPacket::ID, handleShowAsset});
}

void ServerState::processUDPPacket(std::string packet_id, std::string &buffer,
                                   struct addrinfo *connection_addr,
                                   ServerState state) {
    if (udp_packets_handler.count(packet_id) == 0) {
        std::cerr << UNKNOWN_PACKET_ERR << std::endl;
        return;
    }

    udp_packets_handler[packet_id](buffer, connection_addr, state);
}

void ServerState::processTCPPacket(std::string packet_id, int connection_fd) {
    if (tcp_packets_handler.count(packet_id) == 0) {
        std::cerr << UNKNOWN_PACKET_ERR << std::endl;
        return;
    }

    tcp_packets_handler[packet_id](connection_fd);
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
