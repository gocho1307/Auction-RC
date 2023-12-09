#include "server.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"
#include "packets.hpp"
#include "server_state.hpp"

#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <sys/wait.h>
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

    // Get both UDP and TCP listeners running
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "[ERR] Failed to fork into UDP and TCP listeners."
                  << std::endl;
        return EXIT_FAILURE;
    } else if (pid == 0) { // Child (TCP listener)
        while (!state.shutDown) {
            mainTCP();
        }
        return EXIT_SUCCESS;
    } else { // Parent (UDP listener)
        while (!state.shutDown) {
            mainUDP();
        }
    }

    wait(NULL);
    std::cout << std::endl << SHUTDOWN_SERVER << std::endl;

    return EXIT_SUCCESS;
}

void mainUDP() {
    // LIN_LEN is the max size of a UDP message the server should receive
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
    if (n <= PACKET_ID_LEN || n == LIN_LEN) { // invalid Packet size
        ERRUDPPacket err;
        sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                      state.socketUDP);
        std::cerr << PACKET_ERR << std::endl;
        return;
    }

    char strAddr[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &UDPFrom.addr.sin_addr, strAddr, INET_ADDRSTRLEN);
    std::cout << UDP_CONNECTION << strAddr << ":"
              << ntohs(UDPFrom.addr.sin_port) << std::endl;

    std::string msg(buffer);
    interpretUDPPacket(state, msg, UDPFrom);
}

void mainTCP() {
    Address TCPFrom;
    int fd = accept(state.socketTCP, (struct sockaddr *)&TCPFrom.addr,
                    &TCPFrom.addrlen);
    if (fd < 0) {
        if (errno == EAGAIN) { // timeout
            return;
        }
        std::cerr << TCP_ACCEPT_ERR << std::endl;
        return;
    }

    char addr_str[INET_ADDRSTRLEN + 1] = {0};
    inet_ntop(AF_INET, &TCPFrom.addr.sin_addr, addr_str, INET_ADDRSTRLEN);
    std::cout << TCP_CONNECTION << addr_str << ":"
              << ntohs(TCPFrom.addr.sin_port) << std::endl;

    // TODO: handle packet somehow here

    close(fd);
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
