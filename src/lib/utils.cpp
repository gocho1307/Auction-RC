#include "utils.hpp"
#include "messages.hpp"

#include <cctype>
#include <csignal>
#include <cstdint>
#include <cstring>
#include <filesystem>
#include <iostream>
#include <sstream>

int toInt(std::string intStr, uint32_t &num) {
    try {
        size_t conv = 0;
        int64_t resNum = std::stoll(intStr, &conv, 10);
        if (conv != intStr.length() || resNum < 0 || resNum > INT32_MAX) {
            return 1;
        }
        num = (uint32_t)resNum;
    } catch (...) {
        return 1;
    }
    return 0;
}

std::string toDate(time_t seconds) {
    struct tm *time = gmtime(&seconds);
    std::stringstream date;
    date << time->tm_year + 1900 << "-" << std::setfill('0') << std::setw(2)
         << time->tm_mon + 1 << "-" << std::setfill('0') << std::setw(2)
         << time->tm_mday << " " << std::setfill('0') << std::setw(2)
         << time->tm_hour << ":" << std::setfill('0') << std::setw(2)
         << time->tm_min << ":" << std::setfill('0') << std::setw(2)
         << time->tm_sec;
    return date.str();
}

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

int checkUID(std::string uid) {
    if (uid.length() != UID_LEN) {
        std::cerr << UID_ERR << std::endl;
        return 1;
    }
    for (char c : uid) {
        if (!isdigit(c)) {
            std::cerr << UID_ERR << std::endl;
            return 1;
        }
    }
    return 0;
}

int checkPassword(std::string password) {
    if (password.length() != PASSWORD_LEN) {
        std::cerr << PASSWORD_ERR << std::endl;
        return 1;
    }
    for (char c : password) {
        if (!isalnum(c)) {
            std::cerr << PASSWORD_ERR << std::endl;
            return 1;
        }
    }
    return 0;
}

int checkAID(std::string aid) {
    if (aid.length() != AID_LEN) {
        std::cerr << AID_ERR << std::endl;
        return 1;
    }
    for (char c : aid) {
        if (!isdigit(c)) {
            std::cerr << AID_ERR << std::endl;
            return 1;
        }
    }
    return 0;
}

int checkAuctionName(std::string auctionName) {
    if (auctionName.length() > MAX_AUCTION_NAME_LEN) {
        std::cerr << NAME_ERR << std::endl;
        return 1;
    }
    for (char c : auctionName) {
        if (!isalnum(c) && c != '-' && c != '_') {
            std::cerr << NAME_ERR << std::endl;
            return 1;
        }
    }
    return 0;
}

int checkFilePath(std::string fPath) {
    if (!std::filesystem::exists(fPath)) {
        std::cerr << FILE_EXISTANCE_ERR << std::endl;
        return 1;
    }
    std::string fName = std::filesystem::path(fPath).filename();
    if (checkFileName(fName)) {
        return 1;
    }
    return 0;
}

int checkFileName(std::string fName) {
    size_t fNameLen = fName.length();
    if (fNameLen > MAX_FILE_NAME_LEN) {
        std::cerr << FILE_NAME_ERR << std::endl;
        return 1;
    }
    char c;
    for (size_t i = 0; i < fNameLen; ++i) {
        c = fName.at(i);
        if (!isalnum(c) && c != '-' && c != '_' && c != '.') {
            std::cerr << FILE_NAME_ERR << std::endl;
            return 1;
        }
        if (i == fNameLen - FILE_EXTENSION_LEN - 1 && c != '.') {
            std::cerr << FILE_NAME_ERR << std::endl;
            return 1;
        }
    }
    return 0;
}

int checkCalDate(std::string calDate) {
    struct tm t;
    if (calDate.empty() || calDate.length() != CAL_DATE_LEN ||
        !strptime(calDate.c_str(), "%Y-%m-%d", &t)) {
        std::cerr << CAL_DATE_ERR << std::endl;
        return 1;
    }
    return 0;
}

int checkTimeDate(std::string timeDate) {
    struct tm t;
    if (timeDate.empty() || timeDate.length() != TIME_DATE_LEN ||
        !strptime(timeDate.c_str(), "%H:%M:%S", &t)) {
        std::cerr << TIME_DATE_ERR << std::endl;
        return 1;
    }
    return 0;
}

void setupSigHandlers(void (*sigF)(int)) {
    struct sigaction s;

    memset(&s, 0, sizeof(s));
    s.sa_handler = sigF;
    if (sigaction(SIGINT, &s, NULL) == -1 ||
        sigaction(SIGTERM, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }

    memset(&s, 0, sizeof(s));
    s.sa_handler = SIG_IGN;
    if (sigaction(SIGCHLD, &s, NULL) == -1 ||
        sigaction(SIGPIPE, &s, NULL) == -1) {
        std::cerr << SIGACTION_ERR << std::endl;
        return;
    }
}
