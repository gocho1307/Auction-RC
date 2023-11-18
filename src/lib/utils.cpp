#include "utils.hpp"
#include "messages.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>
#include <string>

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
    char c;
    do {
        c = line.front();
        if (c != ' ' && c != '\n' && c != '\0') {
            str += c;
            line.erase(line.begin());
        }
    } while (c != ' ' && c != '\n' && c != '\0');
    while (ignSpaces && c == ' ') {
        line.erase(line.begin());
        c = line.front();
    }
    return str;
}

int readInt(std::string &line, bool ignSpaces) {
    return std::stoi(readString(line, ignSpaces));
}
