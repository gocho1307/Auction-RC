#include "commands.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <iostream>

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

std::string readToken(std::string &line) {
    std::string str = "";
    char c;
    while (!line.empty() && !isspace(c = line.front())) {
        str.push_back(c);
        line.erase(line.begin());
    }
    while (!line.empty() && isspace(line.front())) {
        line.erase(line.begin());
    }
    return str;
}

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
    std::string commandName = readToken(state.line);
    if (commandName.empty() && state.line.empty()) {
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
    std::string uid = readToken(state.line);
    if (checkUID(uid) == -1) {
        return;
    }
    std::string password = readToken(state.line);
    if (checkPassword(password) == -1) {
        return;
    }

    LINPacket packetOut;
    packetOut.UID = uid;
    packetOut.password = password;
    RLIPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLI_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << LOGIN_OK << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << LOGIN_NOK(uid) << std::endl;
        return;
    } else if (packetIn.status == "REG") {
        std::cout << LOGIN_REG(uid) << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
        return;
    }
    state.loggedIn = true;
    state.UID = uid;
    state.password = password;
}

void logoutCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LOUPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    RLOPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLO_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << LOGOUT_OK << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << NO_LOGIN << std::endl;
    } else if (packetIn.status == "UNR") {
        std::cerr << NO_REG(state.UID) << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
    state.loggedIn = false;
}

void unregisterCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    UNRPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    RURPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RUR_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << UNREG_OK(state.UID) << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << NO_LOGIN << std::endl;
    } else if (packetIn.status == "UNR") {
        std::cerr << NO_REG(state.UID) << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
    state.loggedIn = false;
}

void exitCommand(UserState &state) {
    if (state.loggedIn) {
        std::cerr << EXIT_ERR << std::endl;
        return;
    }
    state.shutDown = true;
}

void openCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string auctionName = readToken(state.line);
    if (checkAuctionName(auctionName) == -1) {
        return;
    }
    std::string fPath = readToken(state.line);
    if (checkFilePath(fPath) == -1) {
        return;
    }
    // TODO: check if startValue can be equal to 0
    int startValue;
    if (toInt(readToken(state.line), startValue) == -1 ||
        startValue > MAX_START_VAL || startValue <= 0) {
        std::cerr << START_VAL_ERR << std::endl;
        return;
    }
    // TODO: check if timeActive can be equal to 0
    int timeActive;
    if (toInt(readToken(state.line), timeActive) == -1 ||
        timeActive > MAX_DURATION || timeActive <= 0) {
        std::cerr << DURATION_ERR << std::endl;
        return;
    }

    OPAPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.auctionName = auctionName;
    packetOut.assetfPath = fPath;
    packetOut.startValue = startValue;
    packetOut.timeActive = timeActive;
    ROAPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << OPEN_OK(packetIn.AID) << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << OPEN_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void closeCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string aid = readToken(state.line);
    if (checkAID(aid) == -1) {
        return;
    }

    CLSPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.AID = aid;
    RCLPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << CLOSE_OK << std::endl;
    } else if (packetIn.status == "NLG") {
        std::cout << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "EAU") {
        std::cout << CLOSE_EAU << std::endl;
    } else if (packetIn.status == "EOW") {
        std::cout << CLOSE_EOW << std::endl;
    } else if (packetIn.status == "END") {
        std::cout << CLOSE_END << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void myAuctionsCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LMAPacket packetOut;
    packetOut.UID = state.UID;
    RMAPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RMA_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << MYAUCTIONS_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NLG") {
        std::cout << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cout << MYAUCTIONS_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void myBidsCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LMBPacket packetOut;
    packetOut.UID = state.UID;
    RMBPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RMB_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << MYBIDS_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NLG") {
        std::cout << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cout << MYBIDS_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void listCommand(UserState &state) {
    LSTPacket packetOut;
    RLSPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLS_LEN) == -1) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << LIST_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NOK") {
        std::cout << LIST_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void showAssetCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

void bidCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string aid = readToken(state.line);
    if (checkAID(aid) == -1) {
        return;
    }
    int value;
    if (toInt(readToken(state.line), value) == -1 || value > MAX_START_VAL) {
        std::cerr << START_VAL_ERR << std::endl;
        return;
    }

    BIDPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.AID = aid;
    packetOut.value = value;
    RBDPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn) == -1) {
        return;
    }

    if (packetIn.status == "NOK") {
        std::cout << BID_NOK << std::endl;
    } else if (packetIn.status == "NLG") {
        std::cout << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "ACC") {
        std::cout << BID_ACC << std::endl;
    } else if (packetIn.status == "REF") {
        std::cout << BID_REF << std::endl;
    } else if (packetIn.status == "ILG") {
        std::cout << BID_ILG << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void showRecordCommand(UserState &state) {
    (void)state;
    // TODO: implement
}

// Helper functions

void listAuctions(std::vector<Auction> auctions) {
    std::cout << "------------------------------------------" << std::endl;
    for (Auction auction : auctions) {
        std::cout << "Auction with id '" << auction.AID << "' --------> "
                  << (auction.state == 1 ? "active" : "not active")
                  << std::endl;
    }
}
