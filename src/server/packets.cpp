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

void interpretTCPPacket(const int fd) {
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
    TCPHandler[packetID](fd);
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
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void OPAHandler(const int fd) {
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
    packetOut.serialize(fd);
}

void CLSHandler(const int fd) {
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
    packetOut.serialize(fd);
}

void BIDHandler(const int fd) {
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
    packetOut.serialize(fd);
}

void SASHandler(const int fd) {
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
    packetOut.serialize(fd);
}
