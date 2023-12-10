#include "packets.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"
#include <unistd.h>

UDPPacketsHandler UDPHandler = {{"LIN", LINHandler}, {"LOU", LOUHandler},
                                {"UNR", UNRHandler}, {"LMA", LMAHandler},
                                {"LMB", LMBHandler}, {"LST", LSTHandler},
                                {"SRC", SRCHandler}};
TCPPacketsHandler TCPHandler = {{"OPA", OPAHandler},
                                {"CLS", CLSHandler},
                                {"SAS", SASHandler},
                                {"BID", BIDHandler}};

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom) {
    std::string packetID = msg.substr(0, 3);
    msg.erase(0, 3);

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
    int n = PACKET_ID_LEN;
    while (n-- > 0 && read(fd, &c, 1)) {
        packetID.push_back(c);
    }

    if (packetID.length() != PACKET_ID_LEN ||
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
    } else if (!checkIfUserExists(packetIn.UID)) {
        if (createUserDir(packetIn.UID, packetIn.password)) {
            packetOut.status = "ERR";
        } else {
            packetOut.status = "REG";
        }
    } else if (!checkIfPasswordMatch(packetIn.UID, packetIn.password)) {
        packetOut.status = "NOK";
    } else if (createLogin(packetIn.UID)) {
        packetOut.status = "ERR";
    } else {
        packetOut.status = "OK";
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LOUHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LOUPacket packetIn;
    RLOPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (!checkIfUserExists(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NOK";
    } else if (eraseLogin(packetIn.UID)) {
        packetOut.status = "ERR";
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
    } else if (!checkIfUserExists(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NOK";
    } else if (unregisterUser(packetIn.UID)) {
        packetOut.status = "ERR";
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
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (getUserHostedAuctions(packetIn.UID, auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
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
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (getUserBiddedAuctions(packetIn.UID, auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
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
    } else if (getActiveAuctions(auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void SRCHandler(ServerState &state, std::string msg, Address UDPFrom) {
    (void)state;
    (void)msg;
    (void)UDPFrom;
    // TODO: implement
}

void OPAHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO: implement
}

void CLSHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO: implement
}

void BIDHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO: implement
}

void SASHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO: implement
}
