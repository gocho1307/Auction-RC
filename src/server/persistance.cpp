#include "persistance.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"

#include <filesystem>
#include <fstream>

int checkRegister(const std::string UID) {
    std::string userDir = "USERS/" + UID;
    std::string passPath = userDir + "/password.txt";
    return std::filesystem::exists(userDir) &&
           std::filesystem::exists(passPath);
}

int checkLogin(std::string UID, std::string password) {
    if (!checkRegister(UID)) {
        return 0;
    }
    std::string passPath = "USERS/" + UID + "/password.txt";
    std::ifstream passwordFile(passPath);
    if (!passwordFile.is_open()) {
        std::cerr << FILE_OPEN_ERR(passPath) << std::endl;
        return 0;
    }

    std::string storedPassword;
    passwordFile >> storedPassword;
    passwordFile.close();
    return (storedPassword == password);
}

int registerUser(std::string UID, std::string password) {
    std::string userDir = "USERS/" + UID;
    if (!std::filesystem::exists(userDir)) {
        if (!std::filesystem::create_directory(userDir)) {
            std::cerr << DIR_CREATE_ERR(userDir) << std::endl;
            return 0;
        }
        if (!std::filesystem::create_directory(userDir + "/HOSTED") ||
            !std::filesystem::create_directory(userDir + "/BIDDED")) {
            std::filesystem::remove_all(userDir);
            return 0;
        }
    }

    std::string passPath = userDir + "/password.txt";
    std::ofstream passwordFile(passPath);
    if (!passwordFile.is_open()) {
        std::cerr << FILE_CREATE_ERR(passPath) << std::endl;
        return 0;
    }
    passwordFile << password;
    passwordFile.close();
    return loginUser(UID);
}

int loginUser(std::string UID) {
    std::string loginPath = "USERS/" + UID + "/login.txt";
    std::ofstream loginFile(loginPath);
    if (!std::filesystem::exists(loginPath)) {
        std::cerr << FILE_CREATE_ERR(loginPath) << std::endl;
        return 0;
    }
    loginFile.close();
    return 1;
}

int logoutUser(std::string UID) {
    return std::filesystem::remove("USERS/" + UID + "/login.txt");
}

int unregisterUser(std::string UID) {
    return logoutUser(UID) &&
           std::filesystem::remove("USERS/" + UID + "/password.txt");
}

// TODO: check below
// ----------------------------------------------------------------

int addHostedToUser(std::string UID, std::string AID, int base_value,
                    int duration) {
    std::string hosted_path = "USERS/" + UID + "/HOSTED/" + AID + ".txt";

    std::ofstream file(hosted_path);
    if (!file.is_open()) {
        return 1;
    }

    file << std::to_string(base_value) + "\n" + std::to_string(duration);
    file.close();
    return 0;
}

int addBiddedToUser(std::string UID, std::string AID, int bid_value) {
    std::string bidded_path = "USERS/" + UID + "/BIDDED/" + AID + ".txt";

    std::ofstream file(bidded_path);
    if (!file.is_open()) {
        return 1;
    }

    file << bid_value;
    file.close();
    return 0;
}

int getUserHostedAuctions(std::string UID, std::vector<Auction> &auctions) {
    std::string hosted_by_user_dir = "USERS/" + UID + "/HOSTED";

    DIR *dir = opendir(hosted_by_user_dir.c_str());
    if (dir == nullptr) {
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG &&
            std::string(entry->d_name).find(".txt") != std::string::npos) {

            Auction new_auction;
            new_auction.AID = entry->d_name;
            new_auction.state =
                (uint8_t)checkIfAuctionIsActive((std::string)entry->d_name) ? 1
                                                                            : 0;
            auctions.push_back(new_auction);
        }
    }

    closedir(dir);
    return 0;
}

int getUserBiddedAuctions(std::string UID, std::vector<Auction> &auctions) {
    std::string bidded_by_user_dir = "USERS/" + UID + "/BIDDED";

    DIR *dir = opendir(bidded_by_user_dir.c_str());
    if (dir == nullptr) {
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_REG &&
            std::string(entry->d_name).find(".txt") != std::string::npos) {

            Auction new_auction;
            new_auction.AID = entry->d_name;
            new_auction.state =
                (uint8_t)checkIfAuctionIsActive((std::string)entry->d_name) ? 1
                                                                            : 0;
            auctions.push_back(new_auction);
        }
    }

    closedir(dir);
    return 0;
}

int getActiveAuctions(std::vector<Auction> &auctions) {
    std::string bidded_by_user_dir = "AUCTIONS";

    DIR *dir = opendir(bidded_by_user_dir.c_str());
    if (dir == nullptr) {
        return 1;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != nullptr) {
        if (entry->d_type == DT_DIR && std::string(entry->d_name) != "." &&
            std::string(entry->d_name) != "..") {

            std::string folderName = entry->d_name;

            if (checkIfAuctionIsActive(entry->d_name)) {
                Auction new_auction;
                new_auction.AID = entry->d_name;
                new_auction.state = (uint8_t)1;
                auctions.push_back(new_auction);
            }
        }
    }

    closedir(dir);
    return 0;
}

int checkIfUserHostedAuction(std::string AID, std::string UID) {
    std::string auction_path = "USERS/" + UID + "HOSTED/" + AID + ".txt";
    return std::filesystem::exists(auction_path);
}

int endAuction(std::string AID) {
    std::string end_text_path = "AUCTIONS/" + AID + "/END_" + AID + ".txt";

    std::ofstream file(end_text_path);
    if (!file.is_open()) {
        return 1;
    }

    file.close();
    return 0;
}

int addBidToAuction(std::string AID, std::string user_that_bid, int bid_value) {
    std::string bid_text_path =
        "AUCTIONS/" + AID + "/BIDS/" + std::to_string(bid_value) + ".txt";

    std::ofstream file(bid_text_path);
    if (!file.is_open()) {
        return 1;
    }

    file << user_that_bid;
    file.close();

    // Write in start text of auction the highest bid, deletes previous value
    // written in the start text
    std::string start_text_path = "AUCTIONS/" + AID + "/START_" + AID + ".txt";

    std::ofstream start_text_file(start_text_path, std::ios::trunc);
    if (!start_text_file.is_open()) {
        return 1;
    }

    start_text_file << bid_value;
    start_text_file.close();
    return 0;
}

int checkIfAuctionExists(std::string AID) {
    std::string auction_dir = "AUCTIONS/" + AID;
    return std::filesystem::exists(auction_dir);
}

int checkIfAuctionIsActive(std::string AID) {
    std::string end_file = "AUCTIONS/" + AID + "/END_" + AID + ".txt";
    return !std::filesystem::exists(end_file);
}

int isNewBidHigher(std::string AID, int bid_value) {
    std::string start_text_path = "AUCTIONS/" + AID + "/START_" + AID + ".txt";

    std::ifstream start_text_file(start_text_path);
    if (!start_text_file.is_open()) {
        return false;
    }

    int current_bid_value;
    start_text_file >> current_bid_value;

    start_text_file.close();
    return bid_value > current_bid_value;
}
