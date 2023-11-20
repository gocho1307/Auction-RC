#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <netdb.h>
#include <unistd.h>

std::map<std::string, status> strToStatus = {
    {"OK", status::OK},   {"NOK", status::NOK}, {"REG", status::REG},
    {"UNR", status::UNR}, {"NLG", status::NLG}, {"EOW", status::EOW},
    {"ACC", status::ACC}, {"ILG", status::ILG}, {"END", status::END},
    {"ERR", status::ERR}};

int sendUDPPacket(UDPPacket &packet, struct addrinfo *res, int fd) {
    if (res == NULL) {
        std::cerr << GETADDRINFO_UDP_ERR << std::endl;
        return -1;
    }

    std::string msg = packet.serialize();
    if (sendto(fd, msg.c_str(), msg.length(), 0, res->ai_addr,
               res->ai_addrlen) == -1) {
        std::cerr << SENDTO_ERR << std::endl;
        return -1;
    }
    return 0;
}

int receiveUDPPacket(std::string &response, struct addrinfo *res, int fd) {
    if (res == NULL) {
        std::cerr << GETADDRINFO_UDP_ERR << std::endl;
        return -1;
    }

    char msg[BUFFER_SIZE] = {0};
    if (recvfrom(fd, msg, BUFFER_SIZE, 0, res->ai_addr, &res->ai_addrlen) ==
        -1) {
        std::cerr << RECVFROM_ERR << std::endl;
        return -1;
    }
    response = msg;
    return 0;
}

int sendTCPPacket(TCPPacket &packet, int fd) {
    std::string msg = packet.serialize();
    const char *ptr = msg.c_str();
    ssize_t bytesLeft = (ssize_t)msg.length();
    ssize_t bytesWritten = 0;
    while (bytesLeft > 0) {
        bytesWritten = write(fd, ptr, (size_t)bytesLeft);
        if (bytesWritten <= 0) {
            std::cerr << WRITE_ERR << std::endl;
            return -1;
        }
        bytesLeft -= bytesWritten;
        ptr += bytesWritten;
    }
    return 0;
}

int receiveTCPPacket(std::string &response, int fd) {
    char c;
    ssize_t bytesRead = 0;
    do {
        bytesRead = read(fd, &c, 1);
        if (bytesRead < 0) {
            std::cerr << READ_ERR << std::endl;
            return -1;
        }
        response.push_back(c);
    } while (bytesRead != 0);
    return 0;
}

// Helper functions

int readDelimiter(std::string &line) {
    char c = line.front();
    if (c != ' ') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    line.erase(line.begin());
    c = line.front();
    if (c == ' ' || c == '\t' || c == '\n' || c == '\0') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

int readStatus(std::string &line, status &stat) {
    std::string statusStr = readString(line, false);
    if (strToStatus.find(statusStr) == strToStatus.end()) {
        std::cerr << STATUS_ERR << std::endl;
        return -1;
    }
    stat = strToStatus[statusStr];
    return 0;
}

int readFileData(std::string &fInfo) {
    (void)fInfo;
    // TODO: implement
    return 0;
}

int writeFileData(std::string &fName, std::string &str) {
    (void)fName;
    (void)str;
    // TODO: implement
    return 0;
}

// Packet methods: used by the user side

std::string LINPacket::serialize() {
    // TODO: implement
    return "";
}

void RLIPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string LOUPacket::serialize() {
    // TODO: implement
    return "";
}

void RLOPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string UNRPacket::serialize() {
    // TODO: implement
    return "";
}

void RURPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

// Packet methods: used by the server side

std::string RLIPacket::serialize() {
    // TODO: implement
    return "";
}

void LINPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string RLOPacket::serialize() {
    // TODO: implement
    return "";
}

void LOUPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string RURPacket::serialize() {
    // TODO: implement
    return "";
}

void UNRPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

// Error messages

std::string ERRUDPPacket::serialize() { return "ERR\n"; }

void ERRUDPPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // unimplemented
}
std::string ERRTCPPacket::serialize() { return "ERR\n"; }

void ERRTCPPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // unimplemented
}
