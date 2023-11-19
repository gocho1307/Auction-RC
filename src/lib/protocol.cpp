#include "protocol.hpp"

void sendUDPPacket(UDPPacket &packet) {
    (void)packet;
    // TODO: implement
}

std::string receiveUDPPacket() {
    // TODO: implement
    return "";
}

void sendTCPPacket(TCPPacket &packet) {
    (void)packet;
    // TODO: implement
}

std::string receiveTCPPacket() {
    // TODO: implement
    return "";
}

// Used by the user side

std::string LINPacket::serialize() {
    // TODO: implement
    return "";
}

void RLIPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string LOUPacket::serialize() {
    // TODO: implement
    return "";
}

void RLOPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string UNRPacket::serialize() {
    // TODO: implement
    return "";
}

void RURPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

// Used by the server side

std::string RLIPacket::serialize() {
    // TODO: implement
    return "";
}

void LINPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string RLOPacket::serialize() {
    // TODO: implement
    return "";
}

void LOUPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}

std::string RURPacket::serialize() {
    // TODO: implement
    return "";
}

void UNRPacket::deserialize(std::string &buffer) {
    (void)buffer;
    // TODO: implement
}
