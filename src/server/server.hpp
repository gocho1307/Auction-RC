#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <iostream>

/*
    Shows how to run the server.
*/
void printHelp(std::ostream &stream, char *programPath);

void tcp();

void awaitUDPPacket();

void awaitTCPPacket();

void handlePacket();

#endif // __SERVER_HPP__
