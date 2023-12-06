#ifndef __SERVER_STATE_HPP__
#define __SERVER_STATE_HPP__

#include <netdb.h>
#include <unordered_map>
#include <string>
#include <sys/socket.h>

#include "../lib/constants.hpp"
#include "../lib/protocol.hpp"

class Address {
  public:
    int socket;
    struct sockaddr_in addr;
    socklen_t size;
};

typedef void (*UDPPacketHandler)(std::string &, struct addrinfo*, class ServerState);
typedef void (*TCPPacketHandler)(int connection_fd);

class ServerState {
    std::unordered_map<std::string, UDPPacketHandler> udp_packets_handler;
    std::unordered_map<std::string, TCPPacketHandler> tcp_packets_handler;

  public:
    std::string port = DEFAULT_AS_PORT;

    struct addrinfo *addrUDP = NULL;
    struct addrinfo *addrTCP = NULL;
    int socketUDP = -1;
    int socketTCP = -1;

    bool shutDown = false;
    bool verbose_mode = false;

    void readOpts(int argc, char *argv[]);
    void resolveServerAddress();
    void registerPacketHandlers();
    void processUDPPacket(std::string packet_id, std::string &buffer,
                          struct addrinfo *connection_addr, ServerState state);
    void processTCPPacket(std::string packet_id, int connection_fd);

    ~ServerState();
};


#endif // __SERVER_STATE_HPP__
