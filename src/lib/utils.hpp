#ifndef __UTILS_HPP__
#define __UTILS_HPP__

#include <cstdint>
#include <netdb.h>
#include <string>

class Address {
  public:
    int socket;
    struct sockaddr_in addr;
    socklen_t addrlen = sizeof(addr);
};

typedef struct {
    std::string AID;
    uint8_t state;
} Auction;

typedef struct {
    std::string bidderUID;
    uint32_t value;
    std::string calDate;
    std::string timeDate;
    uint32_t secTime;
} Bid;

int toInt(std::string intStr, uint32_t &num);

std::string toDate(time_t seconds);

int checkPort(std::string port);

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
