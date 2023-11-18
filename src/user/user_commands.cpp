#include "user_commands.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <cstdint>
#include <iostream>
#include <sstream>

CommandsHandler handler = {{"login", loginCommand},
                           {"logout", logoutCommand},
                           {"unregister", unregisterCommand},
                           {"exit", exitCommand},
                           {"open", openCommand},
                           {"close", closeCommand},
                           {"myauctions", myAuctionsCommand},
                           {"ma", myAuctionsCommand},
                           {"mybids", myBidsCommand},
                           {"mb", myBidsCommand},
                           {"list", listCommand},
                           {"l", listCommand},
                           {"show_asset", showAssetCommand},
                           {"sa", showAssetCommand},
                           {"bid", bidCommand},
                           {"b", bidCommand},
                           {"show_record", showRecordCommand},
                           {"sr", showRecordCommand}};

void helpCommand() {
    std::cout << std::endl << "Available commands:" << std::endl;

    std::cout << "  - login <UID> <password>\t"
              << "Logs a user in by their ID and password. If the UID is new, "
                 "it registers the new user."
              << std::endl;
    std::cout << "  - logout\t\t\t"
              << "Logs out the currently logged in user." << std::endl;
    std::cout << "  - unregister\t\t\t"
              << "Unregisters the currently logged in user." << std::endl;
    std::cout << "  - exit\t\t\t"
              << "Exits the user application." << std::endl
              << std::endl;

    std::cout
        << "  - open <auction_name> <asset_fname> <start_value> <time_active>\t"
        << "Opens a new auction." << std::endl;
    std::cout << "  - close <AID>\t\t\t\t\t\t\t\t"
              << "Closes the specified auction." << std::endl
              << std::endl;

    std::cout << "  - myauctions (ma)\t"
              << "Lists the auctions started by the currently logged in user."
              << std::endl;
    std::cout << "  - mybids (mb)\t\t"
              << "Lists the auctions bidded on by the currently logged in user."
              << std::endl;
    std::cout << "  - list (l)\t\t"
              << "Lists the currently active auctions." << std::endl
              << std::endl;

    std::cout
        << "  - show_asset (sa) <AID>\t"
        << "Transfers the image file associated with the specified auction."
        << std::endl;
    std::cout << "  - bid (b) <AID> <value>\t"
              << "Bids on the specified auction." << std::endl;
    std::cout << "  - show_record (sr) <AID>\t"
              << "Presents information about the specified auction."
              << std::endl
              << std::endl;
}

void interpretCommand(UserState &state) {
    std::string commandName = readString(state.line, true);
    if (state.line.empty()) {
        return; // the user pressed enter
    }

    if (handler.find(commandName) == handler.end()) {
        std::cerr << UNEXPECTED_COMMAND_ERR(commandName) << std::endl;
        return;
    }
    handler[commandName](state);
}

void loginCommand(UserState &state) {
    if (state.loggedIn) {
        std::cerr << LOGIN_ERR << std::endl;
        return;
    }
    int uid;
    try {
        uid = readInt(state.line, true);
    } catch (...) {
        std::cerr << UID_ERR << std::endl;
        return;
    }
    std::string password = readString(state.line, true);

    if (uid < MIN_UID || uid > MAX_UID) {
        std::cerr << UID_ERR << std::endl;
        return;
    }
    if (password.length() != PASSWORD_LEN) {
        std::cerr << PASSWORD_ERR << std::endl;
        return;
    }
    for (char c : password) {
        if (!isalnum(c)) {
            std::cerr << PASSWORD_ERR << std::endl;
            return;
        }
    }
    state.UID = uid;
    state.password = password;

    // TODO: implement packets and response handling
}

void logoutCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void unregisterCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void exitCommand(UserState &state) {
    if (state.loggedIn) {
        std::cerr << EXIT_ERR << std::endl;
        return;
    }
    state.shutDown = true;
}

void openCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void closeCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void myAuctionsCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void myBidsCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void listCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void showAssetCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void bidCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void showRecordCommand(UserState &state) {
    (void)state;
    // TODO: implement
}
