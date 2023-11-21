#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

struct ImageData {
    std::size_t fsize;
    std::vector<char> fdata;
};

int validatePort(std::string port);

std::string readString(std::string &line, bool ignSpaces);

int readInt(std::string &line, int &num, bool ignSpaces);

ImageData getImage(std::string &fname);

void setupSigHandlers(void (*sigF)(int));

void listAuctions(std::string auctions);

#endif // __UTILS_HPP__
