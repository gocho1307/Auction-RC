#ifndef __USER_COMMANDS_HPP
#define __USER_COMMANDS_HPP

#include "user.hpp"

#include <map>
#include <string>

class CommandsManager;

typedef void (CommandsManager::*method)(UserState &);
typedef std::map<std::string, method> CommandsHandler;

class CommandsManager {
    CommandsHandler handler = {
        {"login", &CommandsManager::loginCommand},
        {"logout", &CommandsManager::logoutCommand},
        {"unregister", &CommandsManager::unregisterCommand},
        {"exit", &CommandsManager::exitCommand},
        {"open", &CommandsManager::openCommand},
        {"close", &CommandsManager::closeCommand},
        {"myauctions", &CommandsManager::myAuctionsCommand},
        {"ma", &CommandsManager::myAuctionsCommand},
        {"mybids", &CommandsManager::myBidsCommand},
        {"mb", &CommandsManager::myBidsCommand},
        {"list", &CommandsManager::listCommand},
        {"l", &CommandsManager::listCommand},
        {"show_asset", &CommandsManager::showAssetCommand},
        {"sa", &CommandsManager::showAssetCommand},
        {"bid", &CommandsManager::bidCommand},
        {"b", &CommandsManager::bidCommand},
        {"show_record", &CommandsManager::showRecordCommand},
        {"sr", &CommandsManager::showRecordCommand}};

  public:
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
};

#endif // __USER_COMMANDS_HPP
