#include "user_commands.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <fstream>
#include <iostream>
#include <vector>

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
    int uid;
    if (readInt(state.line, uid, true) == -1) {
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

    LINPacket packetOut;
    packetOut.UID = uid;
    packetOut.password = password;
    RLIPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << LOGIN_OK << std::endl;
        break;
    case status::NOK:
        std::cerr << LOGIN_NOK(uid) << std::endl;
        return;
        break;
    case status::REG:
        std::cout << LOGIN_REG(uid) << std::endl;
        break;
    case status::ERR:
    default:
        std::cerr << PACKET_ERR << std::endl;
        return;
        break;
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
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << LOGOUT_OK << std::endl;
        break;
    case status::NOK:
        std::cerr << NO_LOGIN << std::endl;
        break;
    case status::UNR:
        std::cerr << NO_REG(state.UID) << std::endl;
        break;
    case status::ERR:
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
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
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << UNREG_OK(state.UID) << std::endl;
        break;
    case status::NOK:
        std::cerr << NO_LOGIN << std::endl;
        break;
    case status::UNR:
        std::cerr << NO_REG(state.UID) << std::endl;
        break;
    case status::ERR:
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
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

    std::string name = readString(state.line, true);
    if (name.length() > MAX_NAME_LEN) {
        std::cerr << NAME_ERR << std::endl;
        return;
    }
    for (char c : name) {
        if (!isalnum(c)) {
            std::cerr << NAME_ERR << std::endl;
            return;
        }
    }

    // not quite sure if fname is supposed to be the file path of the client or
    //  the name he wants to give the file. I'm assuming it's file path
    std::string fname = readString(state.line, true);
    ImageData image_info = getImage(fname);
    if (image_info.fdata.empty()) {
        std::cerr << IMAGE_ERR << std::endl;
        return;
    }
    std::size_t fsize = image_info.fsize;
    std::vector<char> fdata = image_info.fdata;

    int start_value;
    if (readInt(state.line, start_value, true) == -1 || start_value > MAX_VAL) {
        std::cerr << START_VAL_ERR << std::endl;
        return;
    }

    int timeactive;
    if (readInt(state.line, timeactive, true) == -1 ||
        timeactive > MAX_DURATION) {
        std::cerr << DURATION_ERR << std::endl;
        return;
    }

    OPAPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.name = name;
    packetOut.fname = fname;
    packetOut.fsize = fsize;
    packetOut.fdata = fdata;
    packetOut.start_value = start_value;
    packetOut.timeactive = timeactive;

    ROAPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << OPEN_OK(packetIn.AID) << std::endl;
        break;
    case status::NOK:
        std::cerr << OPEN_NOK << std::endl;
        break;
    case status::ERR:
    default:
        std::cerr << PACKET_ERR << std::endl;
        return;
        break;
    }
}

void closeCommand(UserState &state) {
    if (!state.loggedIn) {
        std::cerr << NO_LOGIN << std::endl;
        return;
    }

    int aid;
    if (readInt(state.line, aid, true) == -1 || aid > MAX_AID) {
        std::cerr << AID_ERR << std::endl;
    }

    CLSPacket packetOut;
    packetOut.UID = state.UID;
    packetOut.password = state.password;
    packetOut.AID = aid;

    RCLPacket packetIn;
    if (state.sendAndReceiveTCPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << CLOSE_OK << std::endl;
        break;
    case status::NLG:
        std::cout << NOT_LOGGED << std::endl;
        break;
    case status::EAU:
        std::cout << CLOSE_EAU << std::endl;
        break;
    case status::EOW:
        std::cout << CLOSE_EOW << std::endl;
        break;
    case status::END:
        std::cout << CLOSE_END << std::endl;
        break;
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
    }
}

void myAuctionsCommand(UserState &state) {
    LMAPacket packetOut;
    packetOut.UID = state.UID;
    RMAPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << MYAUCTIONS_OK << std::endl;
        listAuctions(packetIn.auctions);
        break;
    case status::NLG:
        std::cout << NOT_LOGGED << std::endl;
        break;
    case status::NOK:
        std::cout << MYAUCTIONS_NOK << std::endl;
        break;
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
    }
}

void myBidsCommand(UserState &state) {
    LMBPacket packetOut;
    packetOut.UID = state.UID;
    RMBPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << MYBIDS_OK << std::endl;
        listAuctions(packetIn.auctions);
        break;
    case status::NLG:
        std::cout << NOT_LOGGED << std::endl;
        break;
    case status::NOK:
        std::cout << MYBIDS_NOK << std::endl;
        break;
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
    }
}

void listCommand(UserState &state) {
    (void)state;

    LSTPacket packetOut;
    RLSPacket packetIn;
    if (state.sendAndReceiveUDPPacket(packetOut, packetIn) == -1) {
        return;
    }

    switch (packetIn.stat) {
    case status::OK:
        std::cout << LIST_OK << std::endl;
        listAuctions(packetIn.auctions);
        break;
    case status::NOK:
        std::cout << LIST_NOK << std::endl;
        break;
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
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

    int aid;
    if (readInt(state.line, aid, true) == -1 || aid > MAX_AID) {
        std::cerr << AID_ERR << std::endl;
    }

    int value;
    if (readInt(state.line, value, true) == -1 || value > MAX_VAL) {
        std::cerr << START_VAL_ERR << std::endl;
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

    switch (packetIn.stat) {
    case status::NOK:
        std::cout << BID_NOK << std::endl;
        break;
    case status::NLG:
        std::cout << NOT_LOGGED << std::endl;
        break;
    case status::ACC:
        std::cout << BID_ACC << std::endl;
        break;
    case status::REF:
        std::cout << BID_REF << std::endl;
        break;
    case status::ILG:
        std::cout << BID_ILG << std::endl;
        break;
    default:
        std::cerr << PACKET_ERR << std::endl;
        break;
    }
}

void showRecordCommand(UserState &state) {
    (void)state;
    // TODO: implement
}