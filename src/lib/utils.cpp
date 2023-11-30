#include "utils.hpp"
#include "messages.hpp"

#include <cctype>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>

int checkPort(std::string port) {
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

std::string readToken(std::string &line, bool ignSpaces) {
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
        num = std::stoi(readToken(line, ignSpaces));
    } catch (...) {
        return -1;
    }
    return 0;
}

int readTime(std::string &line, std::string &time) {
    (void)line;
    (void)time;
    // TODO: implement (maybe struct for time, it depends...)
    return 0;
}

int readSpace(std::string &line) {
    if (line.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    char c = line.front();
    if (c != ' ') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    line.erase(line.begin());
    if (line.empty()) {
        return 0;
    }
    c = line.front();
    if (c == ' ' || c == '\t' || c == '\n') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    return 0;
}

int readNewLine(std::string &line) {
    if (line.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    char c = line.front();
    if (c != '\n') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    line.erase(line.begin());
    if (!line.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    return 0;
}

int readAuctions(std::string &line, std::vector<Auction> &auctions) {

    if (line.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1; 
    }

    char c = line.front();

    while (c != '\n') {
        std::string aid = readToken(line, false);
        if (checkAID(aid) == -1) {
            return -1;
        }
        if (readSpace(line) == -1) {
            return -1;
        }
        int state_number;
        if (readInt(line, state_number, false) == -1 ||
            (state_number != 0 && state_number != 1)) {
            return -1;
        }
        bool state = (state_number == 1);
        c = line.front();
        if (c != ' ') {
            continue;
        }
        if (readSpace(line) == -1) {
            return -1;
        }

        Auction auction_to_add;
        auction_to_add.AID = aid;
        auction_to_add.state = state;

        auctions.push_back(auction_to_add);
    }

    return 0;
}

int checkUID(std::string uid) {
    if (uid.length() != UID_LEN) {
        std::cerr << UID_ERR << std::endl;
        return -1;
    }
    for (char c : uid) {
        if (!isdigit(c)) {
            std::cerr << UID_ERR << std::endl;
            return -1;
        }
    }
    return 0;
}

int checkPassword(std::string password) {
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

int checkAID(std::string aid) {
    if (aid.length() != AID_LEN) {
        std::cerr << AID_ERR << std::endl;
        return -1;
    }
    for (char c : aid) {
        if (!isdigit(c)) {
            std::cerr << AID_ERR << std::endl;
            return -1;
        }
    }
    return 0;
}

int checkAuctionName(std::string auctionName) {
    if (auctionName.length() != MAX_NAME_LEN) {
        std::cerr << NAME_ERR << std::endl;
        return -1;
    }
    for (char c : auctionName) {
        if (!isalnum(c)) {
            std::cerr << NAME_ERR << std::endl;
            return -1;
        }
    }
    return 0;
}

int checkFilePath(std::string fPath) {
    if (!std::filesystem::exists(fPath)) {
        std::cerr << FILE_EXISTANCE_ERR << std::endl;
        return -1;
    }
    std::string fName = std::filesystem::path(fPath).filename();
    if (checkFileName(fName) == -1) {
        return -1;
    }
    return 0;
}

int checkFileName(std::string fName) {
    if (fName.length() != MAX_FILE_NAME_LEN) {
        std::cerr << FILE_NAME_ERR << std::endl;
        return -1;
    }
    char c;
    for (size_t i = 0; i < MAX_FILE_NAME_LEN; i++) {
        c = fName.at(i);
        if (!isalnum(c) || c != '-' || c != '_' ||
            (i == MAX_FILE_NAME_LEN - 4 && c != '.')) {
            std::cerr << FILE_NAME_ERR << std::endl;
            return -1;
        }
    }
    return 0;
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
