#include "server_state.hpp"
#include "./lib/messages.hpp"
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

void ServerState::registerPacketHandlers() {
    // UDP
    udp_packets_handler.insert({LINPacket::ID, handle_login});
    udp_packets_handler.insert({LOUPacket::ID, handle_logout});
    udp_packets_handler.insert({UNRPacket::ID, handle_register});
    udp_packets_handler.insert({LMAPacket::ID, handle_my_auctions});
    udp_packets_handler.insert({LMBPacket::ID, handle_my_bids});
    udp_packets_handler.insert({LSTPacket::ID, handle_list});
    udp_packets_handler.insert({SRCPacket::ID, handle_show_record});

    // TCP
    tcp_packets_handler.insert({OPAPacket::ID, handle_open_bid});
    tcp_packets_handler.insert({CLSPacket::ID, handle_close_bid});
    tcp_packets_handler.insert({BIDPacket::ID, handle_bid});
    tcp_packets_handler.insert({SASPacket::ID, handle_show_asset});
}

void ServerState::processUDPPacket(std::string packet_id, std::string &buffer,
                                   Address &connection_addr) {
    if (udp_packets_handler.count(packet_id) == 0) {
        std::cerr << UNKNOWN_PACKET_ERR << std::endl;
        return;
    }

    udp_packets_handler[packet_id](buffer, connection_addr);
}

void ServerState::processTCPPacket(std::string packet_id, int connection_fd) {
    if (tcp_packets_handler.count(packet_id) == 0) {
        std::cerr << UNKNOWN_PACKET_ERR << std::endl;
        return;
    }

    tcp_packets_handler[packet_id](connection_fd);
}