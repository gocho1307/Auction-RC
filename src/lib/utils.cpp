#include "utils.hpp"
#include "messages.hpp"

#include <cctype>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
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
    if (line.empty()) {
        return "";
    }
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

int readUID(std::string &line, int &uid, bool ignSpaces) {
    if (readInt(line, uid, ignSpaces) == -1 || uid < MIN_UID || uid > MAX_UID) {
        std::cerr << UID_ERR << std::endl;
        return -1;
    }
    return 0;
}

int readPassword(std::string &line, std::string &password, bool ignSpaces) {
    password = readString(line, ignSpaces);
    if (password.length() != PASSWORD_LEN) {
        std::cerr << PASSWORD_ERR << std::endl;
        return -1;
    }
    for (char c : password) {
        if (!isalnum(c)) {
            std::cerr << PASSWORD_ERR << std::endl;
            return -1;
        }
    }
    return 0;
}

int readFile(std::string fPath, FileInfo &fInfo) {
    std::ifstream file(fPath, std::ios::in | std::ios::binary);
    if (!file.is_open() || !file.good()) {
        std::cerr << FILE_ERR << std::endl;
        return -1;
    }

    std::filesystem::path path(fPath);
    std::string name = path.filename();
    unsigned long int size = std::filesystem::file_size(path);
    for (char c : name) {
        if (!isalnum(c) && c != '-' && c != '_' && c != '.') {
            std::cerr << FILE_NAME_ERR << std::endl;
            return -1;
        }
    }
    if (size <= 0 || size >= MAX_FILE_SIZE) {
        std::cerr << FILE_SIZE_ERR << std::endl;
        return -1;
    }
    fInfo.name = name;
    fInfo.size = (int)size;

    char buffer[BUFFER_FILE_SIZE] = {0};
    while (file) {
        file.read(buffer, BUFFER_FILE_SIZE);
        fInfo.data += std::string(buffer);
    }
    file.close();
    return 0;
}

int writeFile(std::string dir, FileInfo fInfo) {
    std::ofstream file(dir + fInfo.name);
    if (!file.is_open() || !file.good()) {
        std::cerr << FILE_ERR << std::endl;
        return -1;
    }

    file.write(fInfo.data.c_str(), fInfo.size);
    if (!file.good()) {
        file.close();
        std::cerr << FILE_ERR << std::endl;
        return -1;
    }
    file.close();
    return 0;
}

void listAuctions(std::string auctionsInfo) {
    (void)auctionsInfo;
    // std::stringstream auctionsStream(auctionsInfo);
    // int aid, flag;
    // while (auctionsStream >> aid >> flag) {
    //     std::cout << "Auction" << aid << " : "
    //               << (flag == 1 ? "Active" : "Not Active") << std::endl;
    // }

    // TODO: needs to check the message syntax while printing, and in case of
    // bad syntax return -1.
}

void listBids(std::string bidsInfo) {
    (void)bidsInfo;
    // TODO: list and verify message syntax while printing bids info from RRC.
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
