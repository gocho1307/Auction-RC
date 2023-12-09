#ifndef __COMMANDS_HPP__
#define __COMMANDS_HPP__

#include "user_state.hpp"

#include <functional>
#include <map>
#include <string>

typedef std::map<std::string, std::function<void(UserState &)>> CommandsHandler;

std::string readToken(std::string &line);

void helpHandler();
void interpretCommand(UserState &state);
void loginHandler(UserState &state);
void logoutHandler(UserState &state);
void unregisterHandler(UserState &state);
void exitHandler(UserState &state);
void openHandler(UserState &state);
void closeHandler(UserState &state);
void myAuctionsHandler(UserState &state);
void myBidsHandler(UserState &state);
void listHandler(UserState &state);
void showAssetHandler(UserState &state);
void bidHandler(UserState &state);
void showRecordHandler(UserState &state);

void listAuctions(std::vector<Auction> auctions);

#endif // __COMMANDS_HPP__
