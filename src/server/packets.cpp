#include "packets.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"

#include <iomanip>
#include <sstream>
#include <unistd.h>

UDPPacketsHandler UDPHandler = {{"LIN ", LINHandler}, {"LOU ", LOUHandler},
                                {"UNR ", UNRHandler}, {"LMA ", LMAHandler},
                                {"LMB ", LMBHandler}, {"LST\n", LSTHandler},
                                {"SRC ", SRCHandler}};
TCPPacketsHandler TCPHandler = {{"OPA ", OPAHandler},
                                {"CLS ", CLSHandler},
                                {"SAS ", SASHandler},
                                {"BID ", BIDHandler}};

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom) {
    std::string packetID = msg.substr(0, PACKET_ID_LEN + 1);
    msg.erase(0, PACKET_ID_LEN + 1);

    if (UDPHandler.find(packetID) == UDPHandler.end()) {
        ERRUDPPacket err;
        sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                      state.socketUDP);
        std::cerr << PACKET_ERR << std::endl;
        return;
    }
    UDPHandler[packetID](state, msg, UDPFrom);
}

void interpretTCPPacket(ServerState &state, const int fd) {
    char c;
    std::string packetID;
    int n = PACKET_ID_LEN + 1;
    while (n-- > 0 && read(fd, &c, 1)) {
        packetID.push_back(c);
    }

    if (packetID.length() != PACKET_ID_LEN + 1 ||
        TCPHandler.find(packetID) == TCPHandler.end()) {
        ERRUDPPacket err;
        err.serialize();
        std::cerr << PACKET_ERR << std::endl;
        return;
    }
    TCPHandler[packetID](state, fd);
}

void LINHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LINPacket packetIn;
    RLIPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (checkRegister(packetIn.UID)) {
        if (checkLoginMatch(packetIn.UID, packetIn.password) &&
            loginUser(packetIn.UID)) {
            packetOut.status = "OK";
        } else {
            packetOut.status = "NOK";
        }
    } else if (registerUser(packetIn.UID, packetIn.password)) {
        packetOut.status = "REG";
    } else {
        packetOut.status = "ERR";
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "User with id: " << packetIn.UID << " asked to login."
                       << std::endl;
    } else {
        state.cverbose << "User with undefined id asked to login." << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LOUHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LOUPacket packetIn;
    RLOPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!checkRegister(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkLoggedIn(packetIn.UID) ||
               !checkLoginMatch(packetIn.UID, packetIn.password) ||
               !logoutUser(packetIn.UID)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "User with id: " << packetIn.UID
                       << " asked to logout." << std::endl;
    } else {
        state.cverbose << "User with undefined id asked to logout."
                       << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void UNRHandler(ServerState &state, std::string msg, Address UDPFrom) {
    UNRPacket packetIn;
    RURPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!checkRegister(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkLoggedIn(packetIn.UID) ||
               !checkLoginMatch(packetIn.UID, packetIn.password) ||
               !unregisterUser(packetIn.UID)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "User with id: " << packetIn.UID
                       << " asked to unregister." << std::endl;
    } else {
        state.cverbose << "User with undefined id asked to unregister."
                       << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LMAHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMAPacket packetIn;
    RMAPacket packetOut;

    std::vector<Auction> auctions;
    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!checkLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (!getAuctions("USERS/" + packetIn.UID + "/HOSTED", auctions)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "User with id: " << packetIn.UID
                       << " asked to list their owned auctions." << std::endl;
    } else {
        state.cverbose
            << "User with undefined id asked to list their owned auctions."
            << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LMBHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMBPacket packetIn;
    RMBPacket packetOut;

    std::vector<Auction> auctions;
    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!checkLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (!getAuctions("USERS/" + packetIn.UID + "/BIDDED", auctions)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "User with id: " << packetIn.UID
                       << " asked to list their bids." << std::endl;
    } else {
        state.cverbose << "User with undefined id asked to list their bids"
                       << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LSTHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LSTPacket packetIn;
    RLSPacket packetOut;

    std::vector<Auction> auctions;
    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!getAuctions("AUCTIONS", auctions)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }

    state.cverbose << "A user asked to list all auctions." << std::endl;

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void SRCHandler(ServerState &state, std::string msg, Address UDPFrom) {
    SRCPacket packetIn;
    RRCPacket packetOut;

    std::string info;
    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!getAuctionRecord(packetIn.AID, info)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.info = info;
    }

    if (!packetIn.AID.empty()) {
        state.cverbose << "A user sent a request for the record of auction: "
                       << packetIn.AID << "." << std::endl;
    } else {
        state.cverbose
            << "A user sent a request for the record of an undefined auction."
            << std::endl;
    }

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void OPAHandler(ServerState &state, int fd) {
    OPAPacket packetIn;
    ROAPacket packetOut;

    std::string newAID = getNewAID();
    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else if (!checkLoggedIn(packetIn.UID) ||
               !checkLoginMatch(packetIn.UID, packetIn.password)) {
        packetOut.status = "NLG";
    } else if (!openAuction(newAID, packetIn.UID, packetIn.auctionName,
                            packetIn.assetfName, packetIn.startValue,
                            packetIn.duration)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.AID = newAID;
    }

    if (!packetIn.UID.empty()) {
        state.cverbose << "A user with id: " << packetIn.UID
                       << " asked to open a new auction." << std::endl;
    } else {
        state.cverbose
            << "A user with an undefined id asked to open a new auction."
            << std::endl;
    }

    packetOut.serialize(fd);
}

void CLSHandler(ServerState &state, const int fd) {
    CLSPacket packetIn;
    RCLPacket packetOut;

    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else if (!checkLoggedIn(packetIn.UID) ||
               !checkLoginMatch(packetIn.UID, packetIn.password)) {
        packetOut.status = "NLG";
    } else if (!checkAuctionExists(packetIn.AID)) {
        packetOut.status = "EAU";
    } else if (!checkUserHostedAuction(packetIn.UID, packetIn.AID)) {
        packetOut.status = "EOW";
    } else if (!closeAuction(packetIn.AID)) {
        packetOut.status = "END";
    } else {
        packetOut.status = "OK";
    }

    if (!packetIn.UID.empty() ||
        !packetIn.AID.empty()) { // in case deserialize fails
        state.cverbose << "A user with id: " << packetIn.UID
                       << " asked to close an auction with identifier: "
                       << packetIn.AID << "." << std::endl;
    } else {
        state.cverbose << "A user with an undefined id asked to close an "
                          "auction with a undefined aid."
                       << std::endl;
    }

    packetOut.serialize(fd);
}

void BIDHandler(ServerState &state, const int fd) {
    BIDPacket packetIn;
    RBDPacket packetOut;

    time_t currentTime;
    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else if (!checkAuctionExpiration(packetIn.AID, currentTime)) {
        packetOut.status = "NOK";
    } else if (!checkLoggedIn(packetIn.UID) ||
               !checkLoginMatch(packetIn.UID, packetIn.password)) {
        packetOut.status = "NLG";
    } else if (checkUserHostedAuction(packetIn.UID, packetIn.AID)) {
        packetOut.status = "ILG";
    } else if (!bidAuction(packetIn.AID, packetIn.UID, packetIn.value,
                           currentTime)) {
        packetOut.status = "REF";
    } else {
        packetOut.status = "ACC";
    }

    if (!packetIn.UID.empty() ||
        !packetIn.AID.empty()) { // in case deserialize fails
        state.cverbose << "A user with id: " << packetIn.UID
                       << " asked to bid on an auction with identifier: "
                       << packetIn.AID << "." << std::endl;
    } else {
        state.cverbose << "A user with an undefined id asked to bid on an "
                          "auction with a undefined aid."
                       << std::endl;
    }

    packetOut.serialize(fd);
}

void SASHandler(ServerState &state, const int fd) {
    SASPacket packetIn;
    RSAPacket packetOut;

    std::string fPath;
    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else if (!getAuctionAsset(packetIn.AID, fPath)) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.assetfPath = fPath;
    }

    if (!packetIn.AID.empty()) {
        state.cverbose << "A user asked to receive the image illustrating the "
                          "auction with identifier: "
                       << packetIn.AID << "." << std::endl;
    } else {
        state.cverbose << "A user asked to receive the image illustrating an "
                          "auction with a undefined aid."
                       << std::endl;
    }

    packetOut.serialize(fd);
}
