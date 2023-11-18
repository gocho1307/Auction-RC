#include "user.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"
#include "user_commands.hpp"

#include <cstring>
#include <iostream>
#include <netdb.h>
#include <signal.h>
#include <sstream>
#include <stdlib.h>

UserState state;

int main(int argc, char *argv[]) {
    setupSigHandlers(shutDownSigHandler);

    state.readOpts(argc, argv);
    validatePort(state.port);
    state.getServerAddresses();
    if ((state.socketUDP = socket(AF_INET, SOCK_DGRAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << std::endl;
        return EXIT_FAILURE;
    }

    if (state.host.compare(DEFAULT_AS_HOST) == 0) {
        std::cout << DEFAULT_AS_HOST_STR << std::endl;
    }
    if (state.port.compare(DEFAULT_AS_PORT) == 0) {
        std::cout << DEFAULT_AS_PORT_STR << std::endl;
    }
    state.printTitle();
    helpCommand();

    while (!std::cin.eof() && !state.shutDown) {
        std::cout << PROMPT;
        std::getline(std::cin, state.line);
        interpretCommand(state);
    }

    std::cout << std::endl << SHUTDOWN_STR << std::endl;

    if (state.loggedIn) {
        logoutCommand(state);
    }
    return EXIT_SUCCESS;
}

void UserState::readOpts(int argc, char *argv[]) {
    this->programPath = argv[0];
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
            this->printHelp(std::cout);
            exit(EXIT_SUCCESS);
        default:
            this->printHelp(std::cerr);
            exit(EXIT_FAILURE);
        }
    }
}

void UserState::getServerAddresses() {
    struct addrinfo hints;
    int res;
    const char *host_str = host.c_str();
    const char *port_str = port.c_str();

    // Get UDP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;      // IPv4
    hints.ai_socktype = SOCK_DGRAM; // UDP socket
    if ((res = getaddrinfo(host_str, port_str, &hints, &this->addrUDP)) != 0) {
        std::cerr << GETADDRINFO_UDP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }

    // Get TCP address
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;       // IPv4
    hints.ai_socktype = SOCK_STREAM; // TCP socket
    if ((res = getaddrinfo(host_str, port_str, &hints, &this->addrTCP)) != 0) {
        std::cerr << GETADDRINFO_TCP_ERR << gai_strerror(res) << std::endl;
        exit(EXIT_FAILURE);
    }
}

void UserState::openTCPSocket() {
    if ((this->socketTCP = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        std::cerr << SOCKET_CREATE_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    struct timeval timeout;
    timeout.tv_sec = TCP_READ_TIMEOUT_SECS;
    timeout.tv_usec = 0;
    if (setsockopt(this->socketTCP, SOL_SOCKET, SO_RCVTIMEO, &timeout,
                   sizeof(timeout)) < 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    memset(&timeout, 0, sizeof(timeout));
    timeout.tv_sec = TCP_WRITE_TIMEOUT_SECS;
    if (setsockopt(this->socketTCP, SOL_SOCKET, SO_SNDTIMEO, &timeout,
                   sizeof(timeout)) < 0) {
        std::cerr << SOCKET_TIMEOUT_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
}

void UserState::closeTCPSocket() {
    if (this->socketTCP == -1) {
        return; // socket was already closed
    }
    if (close(this->socketTCP) != 0) {
        std::cerr << SOCKET_CLOSE_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    this->socketTCP = -1;
}

void UserState::printTitle() {
    printf(
        "\n     _   _   _  ____ _____ ___ ___  _   _           ____   ____  \n"
        "    / \\ | | | |/ ___|_   _|_ _/ _ \\| \\ | |         |  _ \\ / ___| "
        "\n"
        "   / _ \\| | | | |     | |  | | | | |  \\| |  _____  | |_) | |     \n"
        "  / ___ \\ |_| | |___  | |  | | |_| | |\\  | |_____| |  _ <| |___  \n"
        " /_/   \\_\\___/ \\____| |_| |___\\___/|_| \\_|         |_| "
        "\\_\\____| \n");
}

void UserState::printHelp(std::ostream &stream) {
    stream << "Usage: " << this->programPath << " [-n ASIP] [-p ASport] [-h]"
           << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-n ASIP\t\tSet hostname of Auction Server. Default is: "
           << DEFAULT_AS_HOST << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default is: "
           << DEFAULT_AS_PORT << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
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

void shutDownSigHandler(int sig) {
    (void)sig;
    state.shutDown = true;
}
