#include "protocol.hpp"
#include "messages.hpp"
#include "utils.hpp"

#include <algorithm>
#include <ctime>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <netdb.h>
#include <string>
#include <unistd.h>

std::string UDPPacket::readString(std::string &buffer) {
    if (buffer.empty()) {
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
        return 1;
    }
    char c = buffer.front();
    if (c != ' ') {
        return 1;
    }
    buffer.erase(buffer.begin());
    if (buffer.empty() || (c = buffer.front()) == ' ' || c == '\n') {
        return 1;
    }
    return 0;
}

int UDPPacket::readNewLine(std::string &buffer) {
    if (buffer.empty()) {
        return 1;
    }
    char c = buffer.front();
    if (c != '\n') {
        return 1;
    }
    buffer.erase(buffer.begin());
    if (!buffer.empty()) {
        return 1;
    }
    return !buffer.empty();
}

int UDPPacket::readAuctions(std::string &buffer,
                            std::vector<Auction> &auctions) {
    if (buffer.empty()) {
        return 1;
    }

    char c;
    while ((c = buffer.front()) != '\n') {
        if (readSpace(buffer)) {
            return 1;
        }
        std::string aid = readString(buffer);
        if (checkAID(aid) || readSpace(buffer)) {
            return 1;
        }
        uint32_t state;
        if (toInt(readString(buffer), state) || (state != 0 && state != 1)) {
            std::cerr << STATE_ERR << std::endl;
            return 1;
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
            return 1;
        }
    } else if (c != ' ') {
        return 1;
    }
    return 0;
}

int TCPPacket::readNewLine(const int fd) {
    char c = delim;
    delim = 0;
    if (c == 0) {
        if (read(fd, &c, 1) != 1 || c != '\n') {
            return 1;
        }
    } else if (c != '\n') {
        return 1;
    }
    return (int)read(fd, &c, 1);
}

int TCPPacket::sendFile(std::string fPath, const int fd) {
    std::ifstream file(fPath, std::ios::in | std::ios::binary);
    if (!file) {
        std::cerr << "Failed to open file: " << fPath << std::endl;
        return 1;
    }
    std::filesystem::path p(fPath);
    std::string fName = p.filename();
    size_t fSize = (size_t)std::filesystem::file_size(p);
    std::string msg = fName + " " + std::to_string((int)fSize) + " ";
    if (sendTCPPacket(msg.c_str(), msg.length(), fd)) {
        file.close();
        std::cerr << FILE_ERR << std::endl;
        return 1;
    }

    ssize_t read;
    ssize_t sent;
    char buffer[FILE_BUFFER_SIZE];
    std::cout << "Upload is in progress..." << std::endl;
    while (file) {
        file.read(buffer, FILE_BUFFER_SIZE);
        read = (ssize_t)file.gcount();
        sent = 0;
        while (sent < read) {
            ssize_t n = write(fd, buffer + sent, (size_t)(read - sent));
            if (n < 0) {
                file.close();
                std::cerr << FILE_ERR << std::endl;
                return 1;
            }
            sent += n;

            // Upload bar
            for (size_t i = 0; i < ((size_t)n / fSize) * 50; ++i) {
                std::cout << "-";
            }
        }
    }
    std::cout << std::endl;
    char newLine = '\n';
    if (sendTCPPacket(&newLine, 1, fd)) {
        file.close();
        std::cerr << FILE_ERR << std::endl;
        return 1;
    }
    file.close();
    return 0;
}

int TCPPacket::receiveFile(std::string fName, size_t fSize, const int fd) {
    std::ofstream file(fName);
    if (!file.good() || !file.is_open()) {
        std::cerr << FILE_ERR << std::endl;
        return 1;
    }
    size_t remaining = fSize;
    size_t toRead;
    ssize_t gotRead;
    char buffer[FILE_BUFFER_SIZE];

    std::cout << "Download is in progress..." << std::endl;
    while (remaining > 0) {
        toRead = std::min(remaining, (size_t)FILE_BUFFER_SIZE);
        gotRead = read(fd, buffer, toRead);
        if (gotRead <= 0) {
            file.close();
            std::cerr << FILE_ERR << std::endl;
            return 1;
        }
        file.write(buffer, gotRead);
        if (!file.good()) {
            file.close();
            std::cerr << FILE_ERR << std::endl;
            return 1;
        }
        remaining -= (size_t)gotRead;

        // Download bar
        for (size_t i = 0; i < ((size_t)gotRead / fSize) * 50; ++i) {
            std::cout << "-";
        }
    }
    std::cout << std::endl;
    file.close();
    return 0;
}

// Packet methods: used by the user side

std::string LINPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLIPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    return readNewLine(buffer);
}

std::string LOUPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RLOPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    return readNewLine(buffer);
}

std::string UNRPacket::serialize() {
    return std::string(ID) + " " + UID + " " + password + "\n";
}

int RURPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    return readNewLine(buffer);
}

int OPAPacket::serialize(const int fd) {
    std::string msg = std::string(ID) + " " + UID + " " + password + " " +
                      auctionName + " " + std::to_string(startValue) + " " +
                      std::to_string(timeActive) + " ";
    if (sendTCPPacket(msg.c_str(), msg.length(), fd)) {
        return 1;
    }
    return sendFile(assetfPath, fd);
}

int ROAPacket::deserialize(const int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID) || readSpace(fd)) {
        return 1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty() || readSpace(fd)) {
        return 1;
    }
    if (status == "OK") {
        AID = readString(fd, AID_LEN);
        if (checkAID(AID)) {
            return 1;
        }
    }
    return readNewLine(fd);
}

int CLSPacket::serialize(const int fd) {
    std::string msg =
        std::string(ID) + " " + UID + " " + password + " " + AID + "\n";
    return sendTCPPacket(msg.c_str(), msg.length(), fd);
}

int RCLPacket::deserialize(const int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID) || readSpace(fd)) {
        return 1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        return 1;
    }
    return readNewLine(fd);
}

std::string LMAPacket::serialize() {
    return std::string(ID) + " " + UID + "\n";
}

int RMAPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    if (status == "OK" && readAuctions(buffer, auctions)) {
        return 1;
    }
    return readNewLine(buffer);
}

std::string LMBPacket::serialize() {
    return std::string(ID) + " " + UID + "\n";
}

int RMBPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    if (status == "OK" && readAuctions(buffer, auctions)) {
        return 1;
    }
    return readNewLine(buffer);
}

std::string LSTPacket::serialize() { return std::string(ID) + "\n"; }

int RLSPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    if (status == "OK" && readAuctions(buffer, auctions)) {
        return 1;
    }
    return readNewLine(buffer);
}

int BIDPacket::serialize(const int fd) {
    std::string msg = std::string(ID) + " " + UID + " " + password + " " + AID +
                      " " + std::to_string(value) + "\n";
    return sendTCPPacket(msg.c_str(), msg.length(), fd);
}

int RBDPacket::deserialize(const int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID) || readSpace(fd)) {
        return 1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        return 1;
    }
    return readNewLine(fd);
}

int SASPacket::serialize(const int fd) {
    std::string msg = std::string(ID) + " " + AID + "\n";
    return sendTCPPacket(msg.c_str(), msg.length(), fd);
}

int RSAPacket::deserialize(const int fd) {
    if (readString(fd, PACKET_ID_LEN) != std::string(ID) || readSpace(fd)) {
        return 1;
    }
    status = readString(fd, MAX_STATUS_LEN);
    if (status.empty()) {
        return 1;
    }
    if (status == "OK") {
        if (readSpace(fd)) {
            return 1;
        }
        assetfName = readString(fd, MAX_FILE_NAME_LEN);
        if (checkFileName(assetfName)) {
            return 1;
        }
        std::string strfSize = readString(fd, 8);
        if (toInt(strfSize, assetfSize) || assetfSize == 0 ||
            assetfSize > MAX_FILE_SIZE) {
            return 1;
        }
        if (receiveFile(assetfName, assetfSize, fd)) {
            return 1;
        }
    }
    return readNewLine(fd);
}

std::string SRCPacket::serialize() {
    return std::string(ID) + " " + AID + "\n";
}

int RRCPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }
    status = readString(buffer);
    if (status.empty()) {
        return 1;
    }
    if (status != "OK") {
        return readNewLine(buffer);
    }

    if (readSpace(buffer)) {
        return 1;
    }
    hostUID = readString(buffer);
    if (checkUID(hostUID) || readSpace(buffer)) {
        return 1;
    }
    auctionName = readString(buffer);
    if (checkAuctionName(auctionName) || readSpace(buffer)) {
        return 1;
    }
    assetfName = readString(buffer);
    if (checkFileName(assetfName) || readSpace(buffer)) {
        return 1;
    }
    // TODO (previous): check if start value can be zero
    std::string strStartValue = readString(buffer);
    if (toInt(strStartValue, startValue) || startValue > MAX_VAL ||
        readSpace(buffer)) {
        return 1;
    }
    calStartDate = readString(buffer);
    if (checkCalDate(calStartDate) || readSpace(buffer)) {
        return 1;
    }
    timeStartDate = readString(buffer);
    if (checkTimeDate(timeStartDate) || readSpace(buffer)) {
        return 1;
    }
    // TODO (previous): check if time active can be zero
    std::string strTimeActive = readString(buffer);
    if (toInt(strTimeActive, timeActive) || timeActive > MAX_DURATION) {
        return 1;
    }

    if (!buffer.empty() && buffer.front() == '\n') {
        return readNewLine(buffer);
    }
    int i = 0;
    do {
        if (readSpace(buffer)) {
            return 1;
        }
        std::string type = readString(buffer);
        if (type == "B") {
            if (readSpace(buffer)) {
                return 1;
            }
            Bid bid;
            bid.bidderUID = readString(buffer);
            if (checkUID(bid.bidderUID) || readSpace(buffer)) {
                return 1;
            }
            std::string strValue = readString(buffer);
            if (toInt(strValue, bid.value) || bid.value > MAX_VAL ||
                readSpace(buffer)) {
                return 1;
            }
            bid.calDate = readString(buffer);
            if (checkCalDate(bid.calDate) || readSpace(buffer)) {
                return 1;
            }
            bid.timeDate = readString(buffer);
            if (checkTimeDate(bid.timeDate) || readSpace(buffer)) {
                return 1;
            }
            std::string strSecTime = readString(buffer);
            if (toInt(strSecTime, bid.secTime) || bid.secTime > MAX_DURATION) {
                return 1;
            }
            bids.push_back(bid);
        } else if (type == "E") {
            if (readSpace(buffer)) {
                return 1;
            }
            calEndDate = readString(buffer);
            if (checkCalDate(calEndDate) || readSpace(buffer)) {
                return 1;
            }
            timeEndDate = readString(buffer);
            if (checkTimeDate(timeEndDate) || readSpace(buffer)) {
                return 1;
            }
            std::string strEndSecTime = readString(buffer);
            if (toInt(strEndSecTime, endSecTime) || endSecTime > MAX_DURATION) {
                return 1;
            }
            return readNewLine(buffer);
        } else {
            return 1;
        }
    } while (!buffer.empty() && buffer.front() != '\n' &&
             ++i < MAX_BIDS_LISTINGS);
    return readNewLine(buffer);
}

// Packet methods: used by the server side

std::string RLIPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int LINPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    UID = readString(buffer);
    if (UID.empty() || readSpace(buffer)) {
        return 1;
    }

    password = readString(buffer);
    if (password.empty()) {
        return 1;
    }

    return readNewLine(buffer) || !buffer.empty();
}

std::string RLOPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int LOUPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    UID = readString(buffer);
    if (UID.empty() || readSpace(buffer)) {
        return 1;
    }

    password = readString(buffer);
    if (password.empty()) {
        return 1;
    }

    return readNewLine(buffer) || !buffer.empty();
}

std::string RURPacket::serialize() {
    return std::string(ID) + " " + status + "\n";
}

int UNRPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    UID = readString(buffer);
    if (UID.empty() || readSpace(buffer)) {
        return 1;
    }

    password = readString(buffer);
    if (password.empty()) {
        return 1;
    }

    return readNewLine(buffer) || !buffer.empty();
}

int ROAPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int OPAPacket::deserialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RCLPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int CLSPacket::deserialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

std::string RMAPacket::serialize() {
    return std::string(ID) + " " + status + listAuctionsToString(auctions) +
           "\n";
}

int LMAPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    UID = readString(buffer);
    if (UID.empty() || readSpace(buffer)) {
        return 1;
    }
    return readNewLine(buffer) || !buffer.empty();
}

std::string RMBPacket::serialize() {
    return std::string(ID) + " " + status + listAuctionsToString(auctions) +
           "\n";
}

int LMBPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    UID = readString(buffer);
    if (UID.empty()) {
        return 1;
    }

    return readNewLine(buffer) || !buffer.empty();
}

std::string RLSPacket::serialize() {
    return std::string(ID) + " " + status + listAuctionsToString(auctions) +
           "\n";
}

int LSTPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID)) {
        return 1;
    }
    return readNewLine(buffer) || !buffer.empty();
}

int RBDPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int BIDPacket::deserialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int RSAPacket::serialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

int SASPacket::deserialize(const int fd) {
    (void)fd;
    // TODO: implement
    return 0;
}

std::string RRCPacket::serialize() {
    // TODO: implement NOW
    return "";
}

int SRCPacket::deserialize(std::string &buffer) {
    if (readString(buffer) != std::string(ID) || readSpace(buffer)) {
        return 1;
    }

    AID = readString(buffer);
    if (AID.empty()) {
        return 1;
    }
    return readNewLine(buffer) || !buffer.empty();
}

std::string ERRUDPPacket::serialize() { return std::string(ID) + "\n"; }

int ERRTCPPacket::serialize(const int fd) {
    std::string msg = std::string(ID) + "\n";
    if (sendTCPPacket(msg.c_str(), msg.length(), fd)) {
        std::cerr << PACKET_ERR << std::endl;
        return 1;
    }
    return 0;
}

// Transmit packets

int sendUDPPacket(UDPPacket &packet, struct addrinfo *res, const int fd) {
    if (res == NULL) {
        std::cerr << GETADDRINFO_UDP_ERR << std::endl;
        return 1;
    }
    std::string msg = packet.serialize();
    if (sendto(fd, msg.c_str(), msg.length(), 0, res->ai_addr,
               res->ai_addrlen) == -1) {
        std::cerr << SENDTO_ERR << std::endl;
        return 1;
    }
    return 0;
}

int receiveUDPPacket(std::string &response, struct addrinfo *res, const int fd,
                     const size_t lim) {
    if (res == NULL) {
        std::cerr << GETADDRINFO_UDP_ERR << std::endl;
        return 1;
    }
    char msg[lim + 1] = {0};
    if (recvfrom(fd, msg, lim, 0, res->ai_addr, &res->ai_addrlen) == -1) {
        std::cerr << RECVFROM_ERR << std::endl;
        return 1;
    }
    response = msg;
    return 0;
}

int sendTCPPacket(const char *msg, const size_t len, const int fd) {
    const char *ptr = msg;
    size_t bytesLeft = len;
    ssize_t bytesWritten = 0;
    while (bytesLeft > 0) {
        bytesWritten = write(fd, ptr, bytesLeft);
        if (bytesWritten <= 0) {
            std::cerr << WRITE_ERR << std::endl;
            return 1;
        }
        bytesLeft -= (size_t)bytesWritten;
        ptr += (size_t)bytesWritten;
    }
    return 0;
}
