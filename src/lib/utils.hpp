#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <cstdint>
#include <string>
#include <vector>

typedef struct {
    std::string AID;
    uint8_t state;
} Auction;

typedef struct {
    std::string bidderUID;
    int value;
    std::string dateTime;
    int secTime;
} Bid;

int checkPort(std::string port);

int toInt(std::string intStr, int &num);

std::string toDate(time_t seconds);

int checkUID(std::string uid);

int checkPassword(std::string password);

int checkAID(std::string aid);

int checkAuctionName(std::string auctionName);

int checkFilePath(std::string fPath);

int checkFileName(std::string fName);

int checkCalDate(std::string calDate);

int checkTimeDate(std::string timeDate);

void setupSigHandlers(void (*sigF)(int));

#endif // __UTILS_HPP__
