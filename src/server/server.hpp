#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include "../lib/protocol.hpp"

#include <iostream>

typedef struct {
    int fd;
    Address address;
    uint32_t time;
} Connection;

void mainUDP();

void mainTCP();

int acceptConnection(Connection &conn);

void printHelp(std::ostream &stream, char *programPath);

void shutDownSigHandler(int sig);

#endif // __SERVER_HPP__
