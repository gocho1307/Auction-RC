#ifndef __SERVER_HPP__
#define __SERVER_HPP__

#include <iostream>

void mainUDP();

void mainTCP();

void printHelp(std::ostream &stream, char *programPath);

void shutDownSigHandler(int sig);

#endif // __SERVER_HPP__
