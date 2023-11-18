#ifndef __USER_COMMANDS_HPP__
#define __USER_COMMANDS_HPP__

#include "user.hpp"

#include <functional>
#include <map>
#include <string>

typedef std::map<std::string, std::function<void(UserState &)>> CommandsHandler;

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

#endif // __USER_COMMANDS_HPP__
