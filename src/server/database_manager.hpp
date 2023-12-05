#ifndef __DATABASE_MANAGER_HPP__
#define __DATABASE_MANAGER_HPP__

#include <string>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sys/stat.h>
#include <unistd.h>

// Users

//int createUserDir(std::string UID, std::string password);
//int createLogin(std::string UID);
// int eraseLogin(std::string UID);

// Auctions

/*
    Saves new auction in database. Returns 0 if successful, 1 otherwise.
*/
int createAuctionDir(int AID);

#endif // __DATABASE_MANAGER_HPP__
