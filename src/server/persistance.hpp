#ifndef __PERSISTANCE_HPP__
#define __PERSISTANCE_HPP__

#include "../lib/utils.hpp"

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <regex>
#include <vector>
#include <dirent.h>

typedef struct {
    std::string name;
    std::string data;
} FileInfo;

int readFile(std::string fPath, FileInfo &fInfo);
int writeFile(std::string dir, FileInfo fInfo);

// All these functions except for bool ones, return 0 if successful, 1 otherwise

// Users
int createUserDir(std::string UID, std::string password);
int createLogin(std::string UID);
int eraseLogin(std::string UID);
int unregisterUser(std::string UID);
int addHostedToUser(std::string UID, std::string AID, int base_value, int duration);
int addBiddedToUser(std::string UID, std::string AID, int bid_value);
int getUserHostedAuctions(std::string UID, std::vector<Auction> &auctions);
int getUserBiddedAuctions(std::string UID, std::vector<Auction> &auctions);
int getActiveAuctions(std::vector<Auction> &auctions);
bool checkIfUserExists(const std::string UID); 
bool checkIfPasswordMatch(std::string UID, std::string password_to_test);
bool checkIfUserHostedAuction(std::string AID, std::string UID);
bool checkIfUserIsLoggedIn(std::string UID);

// Auctions
int createAuctionDir(std::string AID, FileInfo fInfo, int start_value);
int endAuction(std::string AID);
int addBidToAuction(std::string AID, std::string user_that_bid, int bid_value);
bool checkIfAuctionExists(std::string AID);
bool checkIfAuctionIsActive(std::string AID);
bool isNewBidHigher(std::string AID, int bid_value);

#endif // __PERSISTANCE_HPP__
