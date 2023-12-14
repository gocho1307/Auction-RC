#include "packets.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"

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
        if (checkLogin(packetIn.UID, packetIn.password) &&
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
    } else if (checkRegister(packetIn.UID)) {
        if (checkLogin(packetIn.UID, packetIn.password) &&
            logoutUser(packetIn.UID)) {
            packetOut.status = "OK";
        } else {
            packetOut.status = "NOK";
        }
    } else {
        packetOut.status = "UNR";
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void UNRHandler(ServerState &state, std::string msg, Address UDPFrom) {
    UNRPacket packetIn;
    RURPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else if (checkRegister(packetIn.UID)) {
        if (checkLogin(packetIn.UID, packetIn.password) &&
            unregisterUser(packetIn.UID)) {
            packetOut.status = "OK";
        } else {
            packetOut.status = "NOK";
        }
    } else {
        packetOut.status = "UNR";
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
    }
    // TODO: implement

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LMBHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMBPacket packetIn;
    RMBPacket packetOut;
    std::vector<Auction> auctions;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    }
    // TODO: implement

    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LSTHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LSTPacket packetIn;
    RLSPacket packetOut;
    std::vector<Auction> auctions;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    }
    // TODO: implement

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
