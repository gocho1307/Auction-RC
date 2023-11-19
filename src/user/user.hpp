#ifndef __USER_HPP__
#define __USER_HPP__

#include <iostream>

void printTitle();

void printHelp(std::ostream &stream, char *programPath);

void shutDownSigHandler(int sig);

#endif // __USER_HPP__
