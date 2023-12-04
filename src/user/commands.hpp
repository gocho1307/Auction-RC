#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "user_state.hpp"

#include <functional>
#include <map>
#include <string>

typedef std::map<std::string, std::function<void(UserState &)>> CommandsHandler;

std::string readToken(std::string &line);

void helpCommand();
void interpretCommand(UserState &state);
void loginCommand(UserState &state);
void logoutCommand(UserState &state);
void unregisterCommand(UserState &state);
void exitCommand(UserState &state);
void openCommand(UserState &state);
void closeCommand(UserState &state);
void myAuctionsCommand(UserState &state);
void myBidsCommand(UserState &state);
void listCommand(UserState &state);
void showAssetCommand(UserState &state);
void bidCommand(UserState &state);
void showRecordCommand(UserState &state);

void listAuctions(std::vector<Auction> auctions);
void listBids(std::vector<Bid> bids);

#endif // __COMMANDS_HPP__
