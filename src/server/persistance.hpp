#ifndef __PERSISTANCE_HPP__
#define __PERSISTANCE_HPP__

#include "../lib/utils.hpp"

#include <cstring>
#include <dirent.h>
#include <string>
#include <unistd.h>
#include <vector>

int checkRegister(const std::string UID);
int checkLoggedIn(std::string UID);
int checkLoginMatch(std::string UID, std::string password);
int registerUser(std::string UID, std::string password);
int loginUser(std::string UID);
int logoutUser(std::string UID);
int unregisterUser(std::string UID);
void getAuctionTime(std::string AID, uint32_t &fullTime, uint32_t &duration);
int checkAuctionExpiration(std::string AID, time_t &currentTime);
uint8_t getAuctionState(std::string AID);
int getAuctions(std::string path, std::vector<Auction> &auctions);
int closeAuction(std::string AID);
int checkAuctionExists(std::string AID);
int checkUserHostedAuction(std::string UID, std::string AID);
std::string getNewAID();
int openAuction(std::string newAID, std::string UID, std::string auctionName,
                std::string assetfName, uint32_t startValue, uint32_t duration);
int getAuctionRecord(std::string AID, std::string &info);
int bidAuction(std::string AID, std::string UID, uint32_t value,
               time_t currentTime);
int getAuctionAsset(std::string AID, std::string &fPath);

#endif // __PERSISTANCE_HPP__
