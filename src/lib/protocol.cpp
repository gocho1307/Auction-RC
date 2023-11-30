#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <netdb.h>
#include <unistd.h>

std::string UDPPacket::readString(std::string &buffer, uint32_t max_len) {
    std::string str;
    (void) buffer;
    (void) max_len;
    return str;
}

// Packet methods: used by the user side

std::string LINPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLIPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
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
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
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
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string OPAPacket::serialize() {
    return std::string(ID) + " " + UID + " " + auctionName + " " + assetfPath +
           " " + std::to_string(startValue) + "\n";
}

int ROAPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    AID = readToken(buffer, false);
    if (AID.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string CLSPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + " " + AID + "\n";
}

int RCLPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string LMAPacket::serialize() {
    return std::string(ID) + " " + UID + "\n";
}

int RMAPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
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
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
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
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    if (readAuctions(buffer, auctions) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    return readNewLine(buffer);
}

std::string BIDPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + " " + AID + " " +
           std::to_string(value) + "\n";
}

int RBDPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
}

std::string SASPacket::serialize() {
    return std::string(ID) + " " + AID + "\n";
}

int RSAPacket::deserialize(std::string &buffer) {
    if (readToken(buffer, false) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    status = readToken(buffer, false);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(buffer) == -1) {
        return -1;
    }
    assetfPath = readToken(buffer, false);
    if (assetfPath.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(buffer);
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

int sendTCPPacket(const char *msg, const ssize_t len, int fd) {
    const char *ptr = msg;
    ssize_t bytesLeft = len;
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

int receiveTCPPacket(std::string &response, int fd, const ssize_t lim) {
    char c;
    ssize_t bytesRead = 0;
    ssize_t bytesTotal = 0;
    do {
        bytesRead = read(fd, &c, 1);
        if (bytesRead < 0) {
            std::cerr << READ_ERR << std::endl;
            return -1;
        }
        response.push_back(c);
        bytesTotal += bytesRead;
    } while (bytesTotal < lim && bytesRead != 0);
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
    //     fInfo.name = readToken(line, false);
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

// Helper functions

void listAuctions(std::vector<Auction> auctions) {
    (void)auctions;
    // std::stringstream auctionsStream(auctionsInfo);
    // int aid, flag;
    // while (auctionsStream >> aid >> flag) {
    //     std::cout << "Auction" << aid << " : "
    //               << (flag == 1 ? "Active" : "Not Active") << std::endl;
    // }

    // TODO: needs to check the message syntax while printing, and in case of
    // bad syntax return -1.
}

void listBids(std::vector<Bid> bids) {
    (void)bids;
    // TODO: list and verify message syntax while printing bids info from RRC.
}
