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
        std::cerr << "[ERR] Can't create the initial data base directories."
                  << std::endl;
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
            std::cerr << RECVFROM_ERR << std::endl;
            continue;
        }
        char strAddr[INET_ADDRSTRLEN + 1] = {0};
        inet_ntop(AF_INET, &UDPFrom.addr.sin_addr, strAddr, INET_ADDRSTRLEN);
        state.cverbose << UDP_CONNECTION << strAddr << ":"
                       << ntohs(UDPFrom.addr.sin_port) << std::endl;
        if (n <= PACKET_ID_LEN || n == LIN_LEN) { // invalid Packet size
            ERRUDPPacket err;
            sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr,
                          UDPFrom.addrlen, state.socketUDP);
            state.cverbose << "| " << UNKNOWN_MSG << std::endl;
            continue;
        }

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

    std::vector<Connection> conns(FD_SETSIZE);
    int fds, maxfd = state.socketTCP, numConns = 0;
    fd_set tmp, rdfs;
    FD_ZERO(&rdfs);
    FD_SET(state.socketTCP, &rdfs);
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = READ_TIMEOUT_SECS;
    while (!state.shutDown) {
        tmp = rdfs;
        fds = select(maxfd + 1, &tmp, (fd_set *)NULL, (fd_set *)NULL, &tv);
        if (fds == -1) {
            if (errno == EINTR) { // shutDown
                break;
            }
            std::cerr << SELECT_ERR << strerror(errno) << std::endl;
            continue;
        }

        for (int fd = 0; fd < maxfd + 1; ++fd) {
            if (fd != state.socketTCP && FD_ISSET(fd, &rdfs)) {
                Connection conn = conns.at((size_t)fd);
                if ((uint32_t)time(NULL) - conn.time >= READ_TIMEOUT_SECS) {
                    state.cverbose << TCP_REFUSE << conn.host << ":"
                                   << conn.port << std::endl;
                    ERRTCPPacket err;
                    err.serialize(fd);
                    close(fd);
                    FD_CLR(fd, &rdfs);
                    --numConns;
                    continue;
                }
                if (FD_ISSET(fd, &tmp)) {
                    state.cverbose << TCP_CONNECTION << conn.host << ":"
                                   << conn.port << std::endl;
                    interpretTCPPacket(state, fd);
                    close(fd);
                    FD_CLR(fd, &rdfs);
                    --numConns;
                }
            }
        }
        if (FD_ISSET(state.socketTCP, &tmp)) {
            if (numConns++ > MAX_TCP_CONNS) {
                state.cverbose
                    << "Maximum number of concurrent connections reached."
                    << std::endl;
                continue;
            }
            Connection conn;
            if (!acceptConnection(conn)) {
                continue;
            }
            conns.at((size_t)conn.fd) = conn;
            FD_SET(conn.fd, &rdfs);
            maxfd = std::max(maxfd, conn.fd);
        }
    }
    for (int fd = 0; fd < maxfd + 1; ++fd) {
        if (fd != state.socketTCP && FD_ISSET(fd, &rdfs)) {
            Connection conn = conns.at((size_t)fd);
            state.cverbose << TCP_REFUSE << conn.host << ":" << conn.port
                           << std::endl;
            ERRTCPPacket err;
            err.serialize(fd);
            close(fd);
        }
    }
    std::cout << std::endl << SHUTDOWN_TCP_SERVER << std::endl;
}

int acceptConnection(Connection &conn) {
    Address TCPFrom;
    conn.fd = accept(state.socketTCP, (struct sockaddr *)&TCPFrom.addr,
                     &TCPFrom.addrlen);
    if (conn.fd == -1) {
        std::cerr << TCP_ACCEPT_ERR << std::endl;
        return 0;
    }
    struct timeval tv;
    memset(&tv, 0, sizeof(tv));
    tv.tv_sec = WRITE_TIMEOUT_SECS;
    if (setsockopt(conn.fd, SOL_SOCKET, SO_SNDTIMEO, &tv, sizeof(tv)) != 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << strerror(errno) << std::endl;
        return 0;
    }
    conn.time = (uint32_t)time(NULL);

    memset(conn.host, 0, sizeof(conn.host));
    inet_ntop(AF_INET, &TCPFrom.addr.sin_addr, conn.host, INET_ADDRSTRLEN);
    conn.port = ntohs(TCPFrom.addr.sin_port);
    return 1;
}

void printHelp(std::ostream &stream, char *programPath) {
    stream << "Usage: " << programPath << " [-p ASport] [-v] [-h]" << std::endl;
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
