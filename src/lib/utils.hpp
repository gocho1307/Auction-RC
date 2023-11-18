#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

int validatePort(std::string port);

std::string readString(std::string &line, bool ignSpaces);

int readInt(std::string &line, bool ignSpaces);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
