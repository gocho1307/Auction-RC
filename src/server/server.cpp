#include "server.hpp"
#include "../lib/messages.hpp"
#include "packets.hpp"

#include <algorithm>
#include <arpa/inet.h>
#include <cstring>
#include <filesystem>
#include <sys/select.h>
#include <sys/socket.h>
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

    std::filesystem::create_directory("USERS");
    std::filesystem::create_directory("AUCTIONS");
    if (!std::filesystem::exists("USERS") ||
        !std::filesystem::exists("AUCTIONS")) {
        std::cerr << DATA_BASE_ERR << std::endl;
        return EXIT_FAILURE;
    }

    // Get both UDP and TCP listeners running
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "[ERR] Failed to fork into UDP and TCP listeners."
                  << std::endl;
        return EXIT_FAILURE;
    } else if (pid == 0) { // Child (TCP listener)
        mainTCP();
        return EXIT_SUCCESS;
    } else { // Parent (UDP listener)
        mainUDP();
    }

    wait(NULL);
    return EXIT_SUCCESS;
}

void mainUDP() {
    while (!state.shutDown) {
        // LIN_LEN is the max size of a UDP message the server should receive
        char buffer[LIN_LEN + 1] = {0};

        Address UDPFrom;
        ssize_t n =
            recvfrom(state.socketUDP, buffer, LIN_LEN, 0,
                     (struct sockaddr *)&UDPFrom.addr, &UDPFrom.addrlen);
        if (n == -1) {
            if (errno == EINTR) { // shutDown
                break;
            }
            if (errno == EAGAIN) { // timeout
                continue;
            }
            std::cerr << RECVFROM_ERR << std::endl;
            return;
        }
        if (n <= PACKET_ID_LEN || n == LIN_LEN) { // invalid Packet size
            ERRUDPPacket err;
            sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr,
                          UDPFrom.addrlen, state.socketUDP);
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
    std::cout << std::endl << SHUTDOWN_UDP_SERVER << std::endl;
}

void mainTCP() {
    if (listen(state.socketTCP, MAX_TCP_QUEUE) == -1) {
        std::cerr << TCP_LISTEN_ERR << std::endl;
        return;
    }

    int fds, maxfd, newfd = -1;
    fd_set rdfs;
    struct timeval timeout;
    Address TCPFrom;
    while (!state.shutDown) {
        FD_ZERO(&rdfs);
        FD_SET(state.socketTCP, &rdfs);
        maxfd = state.socketTCP;
        if (newfd != -1) {
            FD_SET(newfd, &rdfs);
            maxfd = std::max(maxfd, newfd);
        }

        memset(&timeout, 0, sizeof(timeout));
        timeout.tv_sec = READ_TIMEOUT_SECS;
        fds =
            select(maxfd + 1, &rdfs, (fd_set *)NULL, (fd_set *)NULL, &timeout);
        if (fds == -1) {
            if (errno == EINTR) {
                break;
            }
            std::cerr << SELECT_ERR << strerror(errno) << std::endl;
            return;
        }
        if (fds == 0) { // timeout
            continue;
        }

        if (FD_ISSET(state.socketTCP, &rdfs)) {
            if ((newfd =
                     accept(state.socketTCP, (struct sockaddr *)&TCPFrom.addr,
                            &TCPFrom.addrlen)) == -1) {
                std::cerr << TCP_ACCEPT_ERR << std::endl;
                return;
            }
            timeout.tv_sec = WRITE_TIMEOUT_SECS;
            if (setsockopt(newfd, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                           sizeof(timeout)) != 0) {
                std::cerr << SOCKET_TIMEOUT_ERR << strerror(errno) << std::endl;
                return;
            }
        }
        if (FD_ISSET(newfd, &rdfs)) {
            char strAddr[INET_ADDRSTRLEN + 1] = {0};
            inet_ntop(AF_INET, &TCPFrom.addr.sin_addr, strAddr,
                      INET_ADDRSTRLEN);
            std::cout << TCP_CONNECTION << strAddr << ":"
                      << ntohs(TCPFrom.addr.sin_port) << std::endl;

            interpretTCPPacket(newfd);
            close(newfd);
            newfd = -1;
        }
    }
    std::cout << std::endl << SHUTDOWN_TCP_SERVER << std::endl;
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
