#include "utils.hpp"
#include "messages.hpp"

#include <cctype>
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

int readUID(std::string &line, std::string &uid, bool ignSpaces) {
    uid = readString(line, ignSpaces);
    if (uid.length() != UID_LEN) {
        std::cerr << UID_ERR << std::endl;
        return -1;
    }
    for (char c : uid) {
        if (!isdigit(c)) {
            std::cerr << PASSWORD_ERR << std::endl;
            return -1;
        }
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

int readAID(std::string &line, std::string &aid, bool ignSpaces) {
    aid = readString(line, ignSpaces);
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

int readAuctionName(std::string &line, std::string &auctionName,
                    bool ignSpaces) {
    (void)line;
    (void)auctionName;
    (void)ignSpaces;
    // TODO: implement
    return 0;
}

int readFileName(std::string &line, std::string &fName, bool ignSpaces) {
    (void)line;
    (void)fName;
    (void)ignSpaces;
    // TODO: implement
    return 0;
}

int readFilePath(std::string &line, std::string &fPath, bool ignSpaces) {
    (void)line;
    (void)fPath;
    (void)ignSpaces;
    // TODO: implement
    return 0;
}

int checkFileName(std::string fName) {
    (void)fName;
    // TODO: implement
    return 0;
}

int readTime(std::string &line, std::string &time, bool ignSpaces) {
    (void)line;
    (void)time;
    (void)ignSpaces;
    // TODO: implement
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
