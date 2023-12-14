#ifndef __PERSISTANCE_HPP__
#define __PERSISTANCE_HPP__

#include "../lib/utils.hpp"

#include <cstring>
#include <dirent.h>
#include <iostream>
#include <string>
#include <unistd.h>
#include <vector>

// All these functions return 1 if successful, 0 otherwise

// Users
int checkRegister(const std::string UID);
int checkLogin(std::string UID, std::string password);
int registerUser(std::string UID, std::string password);
int loginUser(std::string UID);
int logoutUser(std::string UID);
int unregisterUser(std::string UID);

int addHostedToUser(std::string UID, std::string AID, int base_value,
                    int duration);
int addBiddedToUser(std::string UID, std::string AID, int bid_value);
int getUserHostedAuctions(std::string UID, std::vector<Auction> &auctions);
int getUserBiddedAuctions(std::string UID, std::vector<Auction> &auctions);
int getActiveAuctions(std::vector<Auction> &auctions);
int checkIfUserHostedAuction(std::string AID, std::string UID);
int checkIfUserIsLoggedIn(std::string UID);

// Auctions
int endAuction(std::string AID);
int addBidToAuction(std::string AID, std::string user_that_bid, int bid_value);
int checkIfAuctionExists(std::string AID);
int checkIfAuctionIsActive(std::string AID);
int isNewBidHigher(std::string AID, int bid_value);

#endif // __PERSISTANCE_HPP__
