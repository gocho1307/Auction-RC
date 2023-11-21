#include "utils.hpp"
#include "messages.hpp"

#include <csignal>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <fstream>
#include <vector>

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

int readInt(std::string &line, int &num, bool ignSpaces) {
    try {
        num = std::stoi(readString(line, ignSpaces));
    } catch (...) {
        return -1;
    }
    return 0;
}

ImageData getImage(std::string &fname) {
    ImageData image_data;

    std::ifstream file(fname, std::ios::binary | std::ios::ate);
    if (!file.is_open()) {
        return image_data;
    }

    image_data.fsize = static_cast<std::size_t>(file.tellg());
    image_data.fdata.resize(image_data.fsize);
    file.seekg(0, std::ios::beg);
    file.read(image_data.fdata.data(), image_data.fsize);
    file.close();

    return image_data;
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
