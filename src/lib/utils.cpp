#include "utils.hpp"
#include "messages.hpp"

#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>

int validatePort(std::string port) {
    if (port.empty()) {
        std::cerr << PORT_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    try {
        int32_t portNum = std::stoi(port);
        if (portNum <= 0 || portNum >= (1 << 16)) {
            std::cerr << PORT_ERR << std::endl;
            exit(EXIT_FAILURE);
        }
    } catch (const std::exception &e) {
        std::cerr << PORT_ERR << std::endl;
        exit(EXIT_FAILURE);
    }
    return 0;
}

std::string readString(std::string &line, bool ignSpaces) {
    std::string str;
    char c = line.front();
    while (c != ' ' && c != '\t' && c != '\n' && c != '\0') {
        str += c;
        line.erase(line.begin());
        c = line.front();
    }
    while (ignSpaces && (c == ' ' || c == '\t')) {
        line.erase(line.begin());
        c = line.front();
    }
    return str;
}

int readInt(std::string &line, bool ignSpaces) {
    return std::stoi(readString(line, ignSpaces));
}

void setupSigHandlers(void (*sigF)(int)) {
    struct sigaction s;

    memset(&s, 0, sizeof(s));
    s.sa_handler = sigF;
    if (sigaction(SIGINT, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }
    if (sigaction(SIGTERM, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }

    memset(&s, 0, sizeof(s));
    s.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }
    if (sigaction(SIGPIPE, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }
}
