#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <iostream>
#include <netdb.h>
#include <unistd.h>

std::string UDPPacket::readString(std::string &buffer) {
    if (buffer.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return "";
    }
    std::string str;
    char c;
    while ((c = buffer.front()) != ' ' && c != '\n') {
        str.push_back(c);
        buffer.erase(buffer.begin());
    }
    return str;
}

int UDPPacket::readSpace(std::string &buffer) {
    if (buffer.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    char c = buffer.front();
    if (c != ' ') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    buffer.erase(buffer.begin());
    if (buffer.empty() || (c = buffer.front()) == ' ' || c == '\n') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

int UDPPacket::readNewLine(std::string &buffer) {
    if (buffer.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    char c = buffer.front();
    if (c != '\n') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    buffer.erase(buffer.begin());
    if (!buffer.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

int UDPPacket::readAuctions(std::string &buffer,
                            std::vector<Auction> &auctions) {
    if (buffer.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    char c;
    while ((c = buffer.front()) != '\n') {
        if (readSpace(buffer) == -1) {
            return -1;
        }
        std::string aid = readString(buffer);
        if (checkAID(aid) == -1 || readSpace(buffer) == -1) {
            return -1;
        }
        int state;
        if (toInt(readString(buffer), state) == -1 ||
            (state != 0 && state != 1)) {
            std::cerr << STATE_ERR << std::endl;
            return -1;
        }
        Auction newAuction;
        newAuction.AID = aid;
        newAuction.state = (uint8_t)state;
        auctions.push_back(newAuction);
    }
    return 0;
}

std::string TCPPacket::readString(const int fd, const size_t lim) {
    std::string str;
    size_t i = 0;
    char c = 0;
    while (i < lim && c != ' ' && c != '\n') {
        if (read(fd, &c, 1) != 1) {
            std::cerr << PACKET_ERR << std::endl;
            return "";
        }
        i++;
        str.push_back(c);
    }
    str.pop_back();
    delim = c;
    return str;
}

int TCPPacket::readSpace(const int fd) {
    char c = delim;
    delim = 0;
    if (c == 0) {
        if (read(fd, &c, 1) != 1 || c != ' ') {
            std::cerr << PACKET_ERR << std::endl;
            return -1;
        }
    } else if (c != ' ') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

int TCPPacket::readNewLine(const int fd) {
    char c = delim;
    delim = 0;
    if (c == 0) {
        if (read(fd, &c, 1) != 1 || c != '\n') {
            std::cerr << PACKET_ERR << std::endl;
            return -1;
        }
    } else if (c != '\n') {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

int TCPPacket::sendFile(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

int TCPPacket::receiveFile(std::string &buffer) {
    (void)buffer;
    // TODO: make this use the 'read' function to read from socket
    //     fInfo.name = readString(buffer);
    //     if (readDelimiter(buffer) == -1) {
    //         return -1;
    //     }
    //     if (readInt(buffer, fInfo.size, false) == -1) {
    //         std::cerr << FILE_SIZE_ERR << std::endl;
    //         return -1;
    //     }
    //     if (readDelimiter(buffer) == -1) {
    //         return -1;
    //     }
    //     if (buffer.length() != fInfo.size) {
    //         std::cerr << FILE_SIZE_ERR << std::endl;
    //         return -1;
    //     }
    //     fInfo.data = buffer;
    return 0;
}

// Packet methods: used by the user side

std::string LINPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLIPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string LOUPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLOPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string UNRPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RURPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

int OPAPacket::serialize(const int fd) {
    (void)fd;
    // TODO
    return 0;
}

int ROAPacket::deserialize(int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    AID = readString(fd, AID_LEN);
    if (AID.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
}

int CLSPacket::serialize(const int fd) {
    (void)fd;
    // TODO
    return 0;
}

int RCLPacket::deserialize(int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
}

std::string LMAPacket::serialize() {
    return std::string(ID) + " " + UID + "\n";
}

int RMAPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readAuctions(buffer, auctions) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string LMBPacket::serialize() {
    return std::string(ID) + " " + UID + "\n";
}

int RMBPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readAuctions(buffer, auctions) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string LSTPacket::serialize() { return std::string(ID) + "\n"; }

int RLSPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readAuctions(buffer, auctions) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

int BIDPacket::serialize(const int fd) {
    (void)fd;
    // TODO
    return 0;
}

int RBDPacket::deserialize(int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
}

int SASPacket::serialize(const int fd) {
    (void)fd;
    // TODO
    return 0;
}

int RSAPacket::deserialize(int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readSpace(fd);
}

std::string SRCPacket::serialize() {
    return std::string(ID) + " " + AID + "\n";
}

int RRCPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

// Packet methods: used by the server side

std::string RLIPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int LINPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RLOPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int LOUPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RURPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int UNRPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

int ROAPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int OPAPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RCLPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int CLSPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

std::string RMAPacket::serialize() {
    // TODO: implement
    return "";
}

int LMAPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RMBPacket::serialize() {
    // TODO: implement
    return "";
}

int LMBPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RLSPacket::serialize() {
    // TODO: implement
    return "";
}

int LSTPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

int RBDPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int BIDPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RSAPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int SASPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

std::string RRCPacket::serialize() {
    // TODO: implement
    return "";
}

int SRCPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string ERRUDPPacket::serialize() { return std::string(ID) + "\n"; }

int ERRTCPPacket::serialize(const int fd) {
    std::string msg = std::string(ID) + "\n";
    if (sendTCPPacket(msg.c_str(), msg.length(), fd) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return 0;
}

// Transmit packets // TODO: NEED to be able to send files before receiving

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

int receiveUDPPacket(std::string &response, struct addrinfo *res, int fd,
                     const size_t lim) {
    if (res == NULL) {
        std::cerr << GETADDRINFO_UDP_ERR << std::endl;
        return -1;
    }
    char msg[lim + 1] = {0};
    if (recvfrom(fd, msg, lim, 0, res->ai_addr, &res->ai_addrlen) == -1) {
        std::cerr << RECVFROM_ERR << std::endl;
        return -1;
    }
    response = msg;
    return 0;
}

int sendTCPPacket(const char *msg, const size_t len, int fd) {
    const char *ptr = msg;
    size_t bytesLeft = len;
    ssize_t bytesWritten = 0;
    while (bytesLeft > 0) {
        bytesWritten = write(fd, ptr, bytesLeft);
        if (bytesWritten <= 0) {
            std::cerr << WRITE_ERR << std::endl;
            return -1;
        }
        bytesLeft -= (size_t)bytesWritten;
        ptr += (size_t)bytesWritten;
    }
    return 0;
}
