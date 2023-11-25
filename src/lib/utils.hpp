#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

int validatePort(std::string port);

std::string readString(std::string &line, bool ignSpaces);

int readInt(std::string &line, int &num, bool ignSpaces);

int readUID(std::string &line, std::string &uid, bool ignSpaces);

int readPassword(std::string &line, std::string &password, bool ignSpaces);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
