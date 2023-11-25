#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <netdb.h>
#include <unistd.h>

// Packet methods: used by the user side

std::string LINPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLIPacket::deserialize(std::string &buffer) {
    if (readString(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer, false);
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
    if (readString(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer, false);
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
    if (readString(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readString(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string OPAPacket::serialize() {
    // TODO: implement
    return "";
}

int ROAPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string CLSPacket::serialize() {
    // TODO: implement
    return "";
}

int RCLPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string LMAPacket::serialize() {
    // TODO: implement
    return "";
}

int RMAPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string LMBPacket::serialize() {
    // TODO: implement
    return "";
}

int RMBPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string LSTPacket::serialize() {
    // TODO: implement
    return "";
}

int RLSPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string BIDPacket::serialize() {
    // TODO: implement
    return "";
}

int RBDPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string SASPacket::serialize() {
    // TODO: implement
    return "";
}

int RSAPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string SRCPacket::serialize() {
    // TODO: implement
    return "";
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

std::string ROAPacket::serialize() {
    // TODO: implement
    return "";
}

int OPAPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RCLPacket::serialize() {
    // TODO: implement
    return "";
}

int CLSPacket::deserialize(std::string &buffer) {
    (void)buffer;
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

std::string RBDPacket::serialize() {
    // TODO: implement
    return "";
}

int BIDPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

std::string RSAPacket::serialize() {
    // TODO: implement
    return "";
}

int SASPacket::deserialize(std::string &buffer) {
    (void)buffer;
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

// Transmit packets // TODO: this to put limits to how much we read from sockets

int sendUDPPacket(Packet &packet, struct addrinfo *res, int fd) {
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

int sendTCPPacket(Packet &packet, int fd) {
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

int sendFile(std::string &line) {
    (void)line;
    // TODO: implement
    return 0;
}

// TODO: make this use the 'read' function to read from socket
int receiveFile(std::string &line) {
    (void)line;
    //     fInfo.name = readString(line, false);
    //     if (readDelimiter(line) == -1) {
    //         return -1;
    //     }
    //     if (readInt(line, fInfo.size, false) == -1) {
    //         std::cerr << FILE_SIZE_ERR << std::endl;
    //         return -1;
    //     }
    //     if (readDelimiter(line) == -1) {
    //         return -1;
    //     }
    //     if (line.length() != fInfo.size) {
    //         std::cerr << FILE_SIZE_ERR << std::endl;
    //         return -1;
    //     }
    //     fInfo.data = line;
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
