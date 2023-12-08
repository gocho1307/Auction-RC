#include "server.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"
#include "server_state.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <thread>
#include <unistd.h>

ServerState state;

int main(int argc, char *argv[]) {
    state.readOpts(argc, argv);

    if (state.verbose_mode) {
        std::cout << "Verbose mode is on." << std::endl;
    }

    if ((state.socketUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << std::endl;
        return EXIT_FAILURE;
    }

    if ((state.socketTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << std::endl;
        return EXIT_FAILURE;
    }

    checkPort(state.port);
    state.resolveServerAddress();
    state.registerPacketHandlers();
    // setupSigHandlers(); ---- TODO

    std::thread tcp_thread(tcp); // get both running
    while (!state.shutDown) {
        awaitUDPPacket();
    }

    tcp_thread.join();
    std::cout << std::endl << SHUTDOWN_SERVER << std::endl;

    return EXIT_SUCCESS;
}

void tcp() {
    // TO DO
    if (listen(state.socketTCP, MAX_TCP_QUEUE) < 0) {
        std::cerr << TCP_LISTEN_ERR << std::endl;
        state.shutDown = true;
        return;
    }

    while (!state.shutDown) {
        awaitTCPPacket();
    }
}

void awaitTCPPacket() {
    Address connection_addr;

    connection_addr.size = sizeof(connection_addr.addr);
    int connection_fd =
        accept(state.socketTCP, (struct sockaddr *)&connection_addr.addr,
               &connection_addr.size);

    if (state.shutDown) {
        return;
    }

    if (connection_fd < 0) {
        if (errno ==
            EAGAIN) { // TODO timeout, just go around and keep listening
            return;
        }
        std::cerr << TCP_CONNECION_ERR << std::endl;
    }

    char addr_str[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &connection_addr.addr.sin_addr, addr_str,
              INET_ADDRSTRLEN);
    std::cout << TCP_CONNECTION << addr_str << ":"
              << ntohs(connection_addr.addr.sin_port) << std::endl;

    // handle packet somehow here

    close(connection_fd);
}

void awaitUDPPacket() {
    char buffer[128];
    memset(buffer, 0, 21);

    struct addrinfo *connection_addr = new struct addrinfo;
    memset(connection_addr, 0, sizeof(struct addrinfo));
    connection_addr->ai_family = AF_UNSPEC;
    connection_addr->ai_socktype = SOCK_DGRAM;
    ssize_t n =
        recvfrom(state.socketUDP, buffer, 21, 0, connection_addr->ai_addr,
                 &connection_addr->ai_addrlen);

    if (n == -1) {
        if (errno == EAGAIN) { // timeout
            return;
        }
        std::cerr << UDP_CONNECION_ERR << std::endl;
    }

    // char addr_str[INET_ADDRSTRLEN + 1] = {0};
    // inet_ntop(AF_INET, &connection_addr->sin_addr, addr_str,
    //           INET_ADDRSTRLEN);
    // std::cout << UDP_CONNECTION << addr_str << ":"
    //           << ntohs(connection_addr->sin_port) << std::endl;

    std::string packet_id(buffer, 3);
    std::string buffer_str(buffer);
    buffer_str.erase(0, 3);

    return state.processUDPPacket(packet_id, buffer_str, connection_addr,
                                  state);
}

void printHelp(std::ostream &stream, char *programPath) {
    stream << "Usage: " << programPath << "[-p ASport] [-v]" << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default is: "
           << DEFAULT_AS_PORT << std::endl;
    stream << "-v\t\tTo run the program in verbose mode." << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
}
