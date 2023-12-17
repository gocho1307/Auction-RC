#include "commands.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <iostream>

CommandsHandler handler = {{"login", loginHandler},
                           {"logout", logoutHandler},
                           {"unregister", unregisterHandler},
                           {"exit", exitHandler},
                           {"open", openHandler},
                           {"close", closeHandler},
                           {"myauctions", myAuctionsHandler},
                           {"ma", myAuctionsHandler},
                           {"mybids", myBidsHandler},
                           {"mb", myBidsHandler},
                           {"list", listHandler},
                           {"l", listHandler},
                           {"show_asset", showAssetHandler},
                           {"sa", showAssetHandler},
                           {"bid", bidHandler},
                           {"b", bidHandler},
                           {"show_record", showRecordHandler},
                           {"sr", showRecordHandler},
                           {"help", helpHandler}};

std::string readToken(std::string &line) {
    std::string str = "";
    char c;
    while (!line.empty() && isspace(line.front())) {
        line.erase(line.begin());
    }
    while (!line.empty() && !isspace(c = line.front())) {
        str.push_back(c);
        line.erase(line.begin());
    }
    return str;
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

void helpHandler(UserState &state) {
    (void)state; // not used
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

    std::cout << "  - help\t Shows this message." << std::endl << std::endl;
}

void loginHandler(UserState &state) {
    if (state.loggedIn) {
        std::cerr << LOGIN_ERR << std::endl;
        return;
    }
    std::string uid = readToken(state.line);
    if (checkUID(uid)) {
        return;
    }
    std::string password = readToken(state.line);
    if (checkPassword(password)) {
        return;
    }

    LINPacket packetOut;
    packetOut.UID = uid;
    packetOut.password = password;
    RLIPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLI_LEN)) {
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

void logoutHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LOUPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    RLOPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLO_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << LOGOUT_OK << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "UNR") {
        std::cerr << NO_REG(state.UID) << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
    state.loggedIn = false;
}

void unregisterHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    UNRPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    RURPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RUR_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << UNREG_OK(state.UID) << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "UNR") {
        std::cerr << NO_REG(state.UID) << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
    state.loggedIn = false;
}

void exitHandler(UserState &state) {
    if (state.loggedIn) {
        std::cerr << EXIT_ERR << std::endl;
        return;
    }
    state.shutDown = true;
}

void openHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string auctionName = readToken(state.line);
    if (checkAuctionName(auctionName)) {
        return;
    }
    std::string fPath = readToken(state.line);
    if (checkFilePath(fPath)) {
        return;
    }
    uint32_t startValue;
    if (toInt(readToken(state.line), startValue) || startValue >= MAX_VAL) {
        std::cerr << START_VAL_ERR << std::endl;
        return;
    }
    uint32_t duration;
    if (toInt(readToken(state.line), duration) || duration > MAX_DURATION) {
        std::cerr << DURATION_ERR << std::endl;
        return;
    }

    OPAPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.auctionName = auctionName;
    packetOut.assetfPath = fPath;
    packetOut.startValue = startValue;
    packetOut.duration = duration;
    ROAPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << OPEN_OK(packetIn.AID) << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << OPEN_NOK << std::endl;
    } else if (packetIn.status == "NLG") {
        std::cerr << NOT_LOGGED << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void closeHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string aid = readToken(state.line);
    if (checkAID(aid)) {
        return;
    }

    CLSPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.AID = aid;
    RCLPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << CLOSE_OK << std::endl;
    } else if (packetIn.status == "NLG") {
        std::cerr << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "EAU") {
        std::cerr << CLOSE_EAU << std::endl;
    } else if (packetIn.status == "EOW") {
        std::cerr << CLOSE_EOW << std::endl;
    } else if (packetIn.status == "END") {
        std::cerr << CLOSE_END << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void myAuctionsHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LMAPacket packetOut;
    packetOut.UID = state.UID;
    RMAPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RMA_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << MYAUCTIONS_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NLG") {
        std::cerr << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << MYAUCTIONS_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void myBidsHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    LMBPacket packetOut;
    packetOut.UID = state.UID;
    RMBPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RMB_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << MYBIDS_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NLG") {
        std::cerr << NOT_LOGGED << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << MYBIDS_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void listHandler(UserState &state) {
    LSTPacket packetOut;
    RLSPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RLS_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << LIST_OK << std::endl;
        listAuctions(packetIn.auctions);
    } else if (packetIn.status == "NOK") {
        std::cerr << LIST_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void showAssetHandler(UserState &state) {
    std::string aid = readToken(state.line);
    if (checkAID(aid)) {
        return;
    }

    SASPacket packetOut;
    packetOut.AID = aid;
    RSAPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << SHOW_ASSET_OK(packetIn.assetfName, packetIn.assetfSize)
                  << std::endl;
    } else if (packetIn.status == "NOK") {
        std::cerr << SHOW_ASSET_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
}

void bidHandler(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }
    std::string aid = readToken(state.line);
    if (checkAID(aid)) {
        return;
    }
    uint32_t value;
    if (toInt(readToken(state.line), value) || value > MAX_VAL) {
        std::cerr << VAL_ERR << std::endl;
        return;
    }

    BIDPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.AID = aid;
    packetOut.value = value;
    RBDPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn)) {
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

void showRecordHandler(UserState &state) {
    std::string aid = readToken(state.line);
    if (checkAID(aid)) {
        return;
    }

    SRCPacket packetOut;
    packetOut.AID = aid;
    RRCPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn, RRC_LEN)) {
        return;
    }

    if (packetIn.status == "OK") {
        std::cout << "General information about the requested auction:"
                  << std::endl;
        std::cout << "hosted by: " << packetIn.hostUID
                  << " | auction name: " << packetIn.auctionName
                  << " | asset file name: " << packetIn.assetfName
                  << " | start value: " << packetIn.startValue << std::endl
                  << "start date time: " << packetIn.calStartDate << " "
                  << packetIn.timeStartDate
                  << " | maximum duration: " << packetIn.duration << " seconds"
                  << std::endl;
        if (!packetIn.calEndDate.empty()) {
            std::cout << "end date time: " << packetIn.calEndDate << " "
                      << packetIn.timeEndDate
                      << " | closed after: " << packetIn.endSecTime
                      << " seconds" << std::endl;
        }
        int i = 0;
        for (Bid bid : packetIn.bids) {
            std::cout << "-----------------------------------------------------"
                         "----------"
                      << std::endl;
            std::cout << "Bid number " << ++i << ":" << std::endl;
            std::cout << "bid by: " << bid.bidderUID
                      << " | bid value: " << bid.value << std::endl
                      << "bid date time: " << bid.calDate << " " << bid.timeDate
                      << " | bidded after: " << bid.secTime << " seconds"
                      << std::endl;
        }
    } else if (packetIn.status == "NOK") {
        std::cerr << SHOW_RECORD_NOK << std::endl;
    } else {
        std::cerr << PACKET_ERR << std::endl;
    }
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
