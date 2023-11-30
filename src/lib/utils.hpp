#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <string>
#include <vector>

typedef struct {
    std::string AID;
    bool state;
} Auction;

typedef struct {
    std::string bidderUID;
    int value;
    std::string dateTime;
    int secTime;
} Bid;

int checkPort(std::string port);

std::string readToken(std::string &line, bool ignSpaces);

int readInt(std::string &line, int &num, bool ignSpaces);

int readTime(std::string &line, std::string &time, bool ignSpaces);

int readSpace(std::string &line);

int readNewLine(std::string &line);

int readAuctions(std::string &line, std::vector<Auction> &auctions);

int checkUID(std::string uid);

int checkPassword(std::string password);

int checkAID(std::string aid);

int checkAuctionName(std::string auctionName);

int checkFilePath(std::string fPath);

int checkFileName(std::string fName);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
