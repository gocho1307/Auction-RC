#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "../lib/protocol.hpp"

#include <iostream>

typedef struct {
    int fd;
    uint32_t time;
    char host[INET_ADDRSTRLEN + 1];
    uint16_t port;
} Connection;

void mainUDP();

void mainTCP();

int acceptConnection(Connection &conn);

void printHelp(std::ostream &stream, char *programPath);

void shutDownSigHandler(int sig);

#endif // __SERVER_HPP__
