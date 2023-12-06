#ifndef __PERSISTANCE_HPP__
#define __PERSISTANCE_HPP__

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>
#include <regex>

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
int addHostedToUser(std::string UID, int AID, int base_value, int duration);
int addBiddedToUser(std::string UID, int AID, int bid_value);
bool checkIfUserExists(const std::string UID); 
bool checkIfPasswordMatch(std::string UID, std::string password_to_test);
bool checkIfUserHostedAuction(int AID, std::string UID);

// Auctions
int createAuctionDir(int AID, FileInfo fInfo, int start_value);
int endAuction(int AID);
int addBidToAuction(int AID, std::string user_that_bid, int bid_value);
bool checkIfAuctionExists(int AID);
bool checkIfAuctionIsActive(int AID);
bool isNewBidHigher(int AID, int bid_value);

#endif // __PERSISTANCE_HPP__
