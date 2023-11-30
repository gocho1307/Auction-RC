#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <iostream>
#include <map>
#include <netdb.h>
#include <unistd.h>

std::string UDPPacket::readString(std::string &buffer) {
    if (buffer.empty()) {
        return "";
    }
    std::string str;
    char c = buffer.front();
    while (c != ' ' && c != '\t' && c != '\n' && c != '\0') {
        str += c;
        buffer.erase(buffer.begin());
        c = buffer.front();
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
    if (buffer.empty()) {
        return 0;
    }
    c = buffer.front();
    if (c == ' ' || c == '\t' || c == '\n') {
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

int UDPPacket::readInt(std::string &buffer, int &num) {
    try {
        num = std::stoi(readString(buffer));
    } catch (...) {
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

    char c = buffer.front();

    while (c != '\n') {
        std::string aid = readString(buffer);
        if (checkAID(aid) == -1) {
            return -1;
        }
        if (readSpace(buffer) == -1) {
            return -1;
        }
        int state_number;
        if (readInt(buffer, state_number) == -1 ||
            (state_number != 0 && state_number != 1)) {
            return -1;
        }
        bool state = (state_number == 1);
        c = buffer.front();
        if (c != ' ') {
            continue;
        }
        if (readSpace(buffer) == -1) {
            return -1;
        }

        Auction auction_to_add;
        auction_to_add.AID = aid;
        auction_to_add.state = state;

        auctions.push_back(auction_to_add);
    }

    return 0;
}

std::string TCPPacket::readString(const int fd) {
    std::string str;
    char c = 0;

    while (!isspace(static_cast<unsigned char>(c))) {
        if (read(fd, &c, 1) != 1) {
            std::cerr << PACKET_ERR << std::endl;
            return "";
        }
        str += c;
    }

    str.pop_back();

    return str;
}

int TCPPacket::readSpace(const int fd) {
    char c = 0;

    if (read(fd, &c, 1) != 1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    if (c != ' ')
        return -1;
    return 0;
}

int TCPPacket::readNewLine(const int fd) {
    char c = 0;

    if (read(fd, &c, 1) != 1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    if (c != '\n')
        return -1;
    return 0;
}

int TCPPacket::readInt(const int fd, int &num) {
    try {
        num = std::stoi(readString(fd));
    } catch (...) {
        return -1;
    }
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

int OPAPacket::serialize(std::string &output) {
    output = std::string(ID) + " " + UID + " " + auctionName + " " +
             assetfPath + " " + std::to_string(startValue) + "\n";
    if (output.length() > OPA_LEN)
        return -1;
    return 0;
}

int ROAPacket::deserialize(int fd) {
    if (readString(fd) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    AID = readString(fd);
    if (AID.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
}

int CLSPacket::serialize(std::string &output) {
    output = std::string(ID) + " " + UID + " " + password + " " + AID + "\n";
    if (output.length() > CLS_LEN)
        return -1;
    return 0;
}

int RCLPacket::deserialize(int fd) {
    if (readString(fd) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd);
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
    if (readSpace(buffer) == -1) {
        return -1;
    }
    if (readAuctions(buffer, auctions) == -1) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }

    return readNewLine(buffer);
}

int BIDPacket::serialize(std::string &output) {
    output = std::string(ID) + " " + UID + " " + password + " " + AID + " " +
             std::to_string(value) + "\n";
    if (output.length() > BID_LEN)
        return -1;
    return 0;
}

int RBDPacket::deserialize(int fd) {
    if (readString(fd) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
}

int SASPacket::serialize(std::string &output) {
    output = std::string(ID) + " " + AID + "\n";
    if (output.length() > SAS_LEN)
        return -1;
    return 0;
}

int RSAPacket::deserialize(int fd) {
    if (readString(fd) != std::string(ID)) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    status = readString(fd);
    if (status.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    if (readSpace(fd) == -1) {
        return -1;
    }
    assetfPath = readString(fd);
    if (assetfPath.empty()) {
        std::cerr << PACKET_ERR << std::endl;
        return -1;
    }
    return readNewLine(fd);
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

int ROAPacket::serialize(std::string &output) {
    (void)output;
    // TODO: implement
    return 0;
}

int OPAPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RCLPacket::serialize(std::string &output) {
    (void)output;
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

int RBDPacket::serialize(std::string &output) {
    (void)output;
    // TODO: implement
    return 0;
}

int BIDPacket::deserialize(int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RSAPacket::serialize(std::string &output) {
    (void)output;
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

int sendFile(std::string &buffer) {
    (void)buffer;
    // TODO: implement
    return 0;
}

// TODO: make this use the 'read' function to read from socket
int receiveFile(std::string &buffer) {
    (void)buffer;
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
