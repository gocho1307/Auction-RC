#include "packets.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"

UDPPacketsHandler UDPHandler = {{"LIN", LINHandler}, {"LOU", LOUHandler},
                                {"UNR", UNRHandler}, {"LMA", LMAHandler},
                                {"LMB", LMBHandler}, {"LST", LSTHandler},
                                {"SRC", SRCHandler}};
TCPPacketsHandler TCPHandler = {{"OPA", OPAHandler},
                                {"CLS", CLSHandler},
                                {"SAS", SASHandler},
                                {"BID", BIDHandler}};

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom) {
    std::string packetID(msg, 3);
    msg.erase(0, 3);

    if (UDPHandler.find(packetID) == UDPHandler.end() ||
        UDPHandler[packetID](state, msg, UDPFrom)) {
        ERRUDPPacket err;
        sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                      state.socketUDP);
        std::cerr << PACKET_ERR << std::endl;
    }
}

void interpretTCPPacket(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO
}

int LINHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LINPacket packetIn;
    RLIPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    if (!checkIfUserExists(packetIn.UID)) {
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
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int LOUHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LOUPacket packetIn;
    RLOPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    if (!checkIfUserExists(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NOK";
    } else if (eraseLogin(packetIn.UID)) {
        packetOut.status = "ERR";
    } else {
        packetOut.status = "OK";
    }
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int UNRHandler(ServerState &state, std::string msg, Address UDPFrom) {
    UNRPacket packetIn;
    RURPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    if (!checkIfUserExists(packetIn.UID)) {
        packetOut.status = "UNR";
    } else if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NOK";
    } else if (unregisterUser(packetIn.UID)) {
        packetOut.status = "ERR";
    } else {
        packetOut.status = "OK";
    }
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int LMAHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMAPacket packetIn;
    RMAPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    std::vector<Auction> auctions;
    if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (getUserHostedAuctions(packetIn.UID, auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int LMBHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMBPacket packetIn;
    RMBPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    std::vector<Auction> auctions;
    if (!checkIfUserIsLoggedIn(packetIn.UID)) {
        packetOut.status = "NLG";
    } else if (getUserBiddedAuctions(packetIn.UID, auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int LSTHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LSTPacket packetIn;
    RLSPacket packetOut;

    if (packetIn.deserialize(msg)) {
        return 1;
    }

    std::vector<Auction> auctions;
    if (getActiveAuctions(auctions)) {
        packetOut.status = "ERR";
    } else if (auctions.empty()) {
        packetOut.status = "NOK";
    } else {
        packetOut.status = "OK";
        packetOut.auctions = auctions;
    }
    return sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr,
                         UDPFrom.addrlen, state.socketUDP);
}

int SRCHandler(ServerState &state, std::string msg, Address UDPFrom) {
    (void)state;
    (void)msg;
    (void)UDPFrom;
    // TODO
    return 0;
}

int OPAHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO
    return 0;
}

int CLSHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO
    return 0;
}

int BIDHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO
    return 0;
}

int SASHandler(ServerState &state, const int fd) {
    (void)state;
    (void)fd;
    // TODO
    return 0;
}
