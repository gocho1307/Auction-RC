#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>

int validatePort(std::string port);

std::string readString(std::string &line, bool ignSpaces);

int readInt(std::string &line, int &num, bool ignSpaces);

int readUID(std::string &line, std::string &uid, bool ignSpaces);

int readPassword(std::string &line, std::string &password, bool ignSpaces);

int readAID(std::string &line, std::string &aid, bool ignSpaces);

int readAuctionName(std::string &line, std::string &auctionName,
                    bool ignSpaces);

int readFileName(std::string &line, std::string &fName, bool ignSpaces);

int readFilePath(std::string &line, std::string &fPath, bool ignSpaces);

int checkFileName(std::string fName);

int readTime(std::string &line, std::string &time, bool ignSpaces);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
