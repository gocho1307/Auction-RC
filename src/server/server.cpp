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
    setupSigHandlers(shutDownSigHandler);

    state.readOpts(argc, argv);
    checkPort(state.port);
    state.openUDPSocket();
    state.openTCPSocket();
    state.getServerAddresses();

    state.cverbose << "[INFO] Verbose mode is activated." << std::endl;

    // Get both UDP and TCP running
    std::thread TCPThread(mainTCP);
    mainUDP();

    TCPThread.join();
    std::cout << std::endl << SHUTDOWN_SERVER << std::endl;

    return EXIT_SUCCESS;
}

void mainUDP() {
    // LIN_LEN is the max size of a UDP message the server can receive
    char buffer[LIN_LEN + 1] = {0};

    Address UDPFrom;
    ssize_t n = recvfrom(state.socketUDP, buffer, LIN_LEN, 0,
                         (struct sockaddr *)&UDPFrom.addr, &UDPFrom.addrlen);
    if (n == -1) {
        if (errno == EAGAIN) { // timeout
            return;
        }
        std::cerr << RECVFROM_ERR << std::endl;
        return;
    }

    // char addr_str[INET_ADDRSTRLEN + 1] = {0};
    // inet_ntop(AF_INET, &connection_addr->sin_addr, addr_str,
    //           INET_ADDRSTRLEN);
    // std::cout << UDP_CONNECTION << addr_str << ":"
    //           << ntohs(connection_addr->sin_port) << std::endl;

    std::string packet_id(buffer, 3);
    std::string buffer_str(buffer);
    buffer_str.erase(0, 3);

    // return state.processUDPPacket(packet_id, buffer_str, connection_addr,
    //                               state);
}

void mainTCP() {
    Address connection_addr;

    int connection_fd =
        accept(state.socketTCP, (struct sockaddr *)&connection_addr.addr,
               &connection_addr.addrlen);

    if (state.shutDown) {
        return;
    }

    if (connection_fd < 0) {
        if (errno ==
            EAGAIN) { // TODO timeout, just go around and keep listening
            return;
        }
        std::cerr << TCP_CONNECTION_ERR << std::endl;
    }

    char addr_str[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &connection_addr.addr.sin_addr, addr_str,
              INET_ADDRSTRLEN);
    std::cout << TCP_CONNECTION << addr_str << ":"
              << ntohs(connection_addr.addr.sin_port) << std::endl;

    // handle packet somehow here

    close(connection_fd);
}

void printHelp(std::ostream &stream, char *programPath) {
    stream << "Usage: " << programPath << "[-p ASport] [-v] [-h]" << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default is: "
           << DEFAULT_AS_PORT << std::endl;
    stream << "-v\t\tTo run the server in verbose mode." << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
}

void shutDownSigHandler(int sig) {
    (void)sig; // not used
    state.shutDown = true;
}
