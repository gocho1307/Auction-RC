#include "packets_handler.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"

void handleLogin(std::string &buffer, struct addrinfo *connection_addr,
                 ServerState state) {
    LINPacket packet_in;
    RLIPacket packet_out;

    packet_in.deserialize(buffer);

    if (!checkIfUserExists(packet_in.UID)) {
        if (createUserDir(packet_in.UID, packet_in.password) == 1) {
            packet_out.status = "ERR";
        } else {
            packet_out.status = "REG";
        }
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (!checkIfPasswordMatch(packet_in.UID, packet_in.password)) {
        packet_out.status = "NOK";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (createLogin(packet_in.UID) == 1) {
        packet_out.status = "ERR";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }

    packet_out.status = "OK";
    sendUDPPacket(packet_out, connection_addr, state.socketUDP);
}

void handleLogout(std::string &buffer, struct addrinfo *connection_addr,
                  ServerState state) {
    LOUPacket packet_in;
    RLOPacket packet_out;

    packet_in.deserialize(buffer);

    if (!checkIfUserExists(packet_in.UID)) {
        packet_out.status = "UNR";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (!checkIfUserIsLoggedIn(packet_in.UID)) {
        packet_out.status = "NOK";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (eraseLogin(packet_in.UID) == 1) {
        packet_out.status = "ERR";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }

    packet_out.status = "OK";
    sendUDPPacket(packet_out, connection_addr, state.socketUDP);
}

void handleRegister(std::string &buffer, struct addrinfo *connection_addr,
                    ServerState state) {
    UNRPacket packet_in;
    RURPacket packet_out;

    packet_in.deserialize(buffer);

    if (!checkIfUserExists(packet_in.UID)) {
        packet_out.status = "UNR";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (!checkIfUserIsLoggedIn(packet_in.UID)) {
        packet_out.status = "NOK";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }
    if (unregisterUser(packet_in.UID) == 1) {
        packet_out.status = "ERR";
        sendUDPPacket(packet_out, connection_addr, state.socketUDP);
        return;
    }

    packet_out.status = "OK";
    sendUDPPacket(packet_out, connection_addr, state.socketUDP);
}

void handleMyAuctions(std::string &buffer, struct addrinfo *connection_addr,
                      ServerState state) {
    LMAPacket packet_in;
    RMAPacket packet_out;
    (void)connection_addr;
    (void)state;

    packet_in.deserialize(buffer);
}

void handleMyBids(std::string &buffer, struct addrinfo *connection_addr,
                  ServerState state) {
    (void)buffer;
    (void)connection_addr;
    (void)state;
    // TO DO
}

void handleList(std::string &buffer, struct addrinfo *connection_addr,
                ServerState state) {
    (void)buffer;
    (void)connection_addr;
    (void)state;
    // TO DO
}

void handleShowRecord(std::string &buffer, struct addrinfo *connection_addr,
                      ServerState state) {
    (void)buffer;
    (void)connection_addr;
    (void)state;
    // TO DO
}

void handleOpenAuction(int connection_fd) {
    (void)connection_fd;
    // TO DO
}

void handleCloseAuction(int connection_fd) {
    (void)connection_fd;
    // TO DO
}

void handleBid(int connection_fd) {
    (void)connection_fd;
    // TO DO
}

void handleShowAsset(int connection_fd) {
    (void)connection_fd;
    // TO DO
}
