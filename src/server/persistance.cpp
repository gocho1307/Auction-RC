#include "persistance.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"

#include <filesystem>
#include <fstream>

int readFile(std::string fPath, FileInfo &fInfo) {
    (void)fPath;
    (void)fInfo;
    // std::ifstream file(fPath, std::ios::in | std::ios::binary);
    // if (!file.is_open() || !file.good()) {
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    //
    // std::filesystem::path path(fPath);
    // std::string name = path.filename();
    // unsigned long int size = std::filesystem::file_size(path);
    // for (char c : name) {
    //     if (!isalnum(c) && c != '-' && c != '_' && c != '.') {
    //         std::cerr << FILE_NAME_ERR << std::endl;
    //         return -1;
    //     }
    // }
    // if (size <= 0 || size >= MAX_FILE_SIZE) {
    //     std::cerr << FILE_SIZE_ERR << std::endl;
    //     return -1;
    // }
    // fInfo.name = name;
    // fInfo.size = (int)size;
    //
    // char buffer[BUFFER_FILE_SIZE] = {0};
    // while (file) {
    //     file.read(buffer, BUFFER_FILE_SIZE);
    //     fInfo.data += std::string(buffer);
    // }
    // file.close();
    return 0;
}

int writeFile(std::string dir, FileInfo fInfo) {
    (void)dir;
    (void)fInfo;
    // std::ofstream file(dir + fInfo.name);
    // if (!file.is_open() || !file.good()) {
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    //
    // file.write(fInfo.data.c_str(), fInfo.size);
    // if (!file.good()) {
    //     file.close();
    //     std::cerr << FILE_ERR << std::endl;
    //     return -1;
    // }
    // file.close();
    return 0;
}

int createUserDir(std::string UID, std::string password) {
    std::string user_dirname = "USERS/" + UID;
    if (std::filesystem::create_directory(user_dirname) == false) {
        return 1;
    }

    std::string hosted_dir = "USERS/" + UID + "/HOSTED";
    if (std::filesystem::create_directory(hosted_dir) == false) {
        std::filesystem::remove_all(user_dirname);
        return 1;
    }

    std::string bidded_dir = "USERS/" + UID + "/BIDDED";
    if (std::filesystem::create_directory(bidded_dir) == false) {
        std::filesystem::remove_all(user_dirname);
        return 1;
    }

    std::string pass_text_path = "USERS/" + UID + "/" + UID + "_pass.txt";
    std::ofstream file(pass_text_path);
    if (!file.is_open()) {
        std::filesystem::remove_all(user_dirname);
        return 1;
    }

    file << password;
    file.close();
    return 0;
}

int createLogin(std::string UID) {
    std::string login_text_path = "USERS/" + UID + "/" + UID + "_login.txt";

    std::ofstream file(login_text_path);
    if (!file.is_open()) {
        return 1;
    }

    file << "Logged in\n";
    file.close();
    return 0;
}

int eraseLogin(std::string UID) {
    std::string login_text_path = "USERS/" + UID + "/" + UID + "_login.txt";

    if (std::remove(login_text_path.c_str()) != 0) {
        return 1;
    }

    return 0;
}

int unregisterUser(std::string UID) {
    std::string user_dir = "USERS/" + UID;

    try {
        std::filesystem::remove_all(user_dir);
    } catch (const std::filesystem::filesystem_error &e) {
        return 1;
    }

    return 0;
}

int addHostedToUser(std::string UID, int AID, int base_value, int duration) {
    std::string hosted_path =
        "USERS/" + UID + "/HOSTED/" + std::to_string(AID) + ".txt";

    std::ofstream file(hosted_path);
    if (!file.is_open()) {
        return 1;
    }

    file << base_value + "\n" + duration;
    file.close();
    return 0;
}

int addBiddedToUser(std::string UID, int AID, int bid_value) {
    std::string bidded_path =
        "USERS/" + UID + "/BIDDED/" + std::to_string(AID) + ".txt";

    std::ofstream file(bidded_path);
    if (!file.is_open()) {
        return 1;
    }

    file << bid_value;
    file.close();
    return 0;
}

bool checkIfUserExists(const std::string UID) {
    std::string user_dir = "USERS/" + UID;
    return std::filesystem::exists(user_dir);
}

bool checkIfPasswordMatch(std::string UID, std::string password_to_test) {
    std::string pass_file_path = "USERS" + UID + "/" + UID + "_pass.txt";

    std::ifstream password_file(pass_file_path);
    if (!password_file.is_open()) {
        std::cerr << "Error opening password file: " << pass_file_path
                  << std::endl;
        return false;
    }

    std::string stored_password;
    password_file >> stored_password;

    password_file.close();
    return (stored_password == password_to_test);
}

bool checkIfUserHostedAuction(int AID, std::string UID) {
    std::string auction_path =
        "USERS/" + UID + "HOSTED/" + std::to_string(AID) + ".txt";
    return std::filesystem::exists(auction_path);
}

int createAuctionDir(int AID, FileInfo fInfo, int start_value) {
    std::string AID_dirname = "AUCTIONS/" + std::to_string(AID);
    if (std::filesystem::create_directory(AID_dirname) == false) {
        return 1;
    }

    std::string ASSET_dirname = AID_dirname + "/ASSET";
    if (std::filesystem::create_directory(ASSET_dirname) == false) {
        std::filesystem::remove_all(AID_dirname);
        return 1;
    }

    if (writeFile(ASSET_dirname, fInfo) == -1) {
        std::filesystem::remove_all(AID_dirname);
        return 1;
    }

    std::string BIDS_dirname = AID_dirname + "/BIDS";
    if (std::filesystem::create_directory(BIDS_dirname) == false) {
        std::filesystem::remove_all(AID_dirname);
        return 1;
    }

    std::string start_text_path = "AUCTIONS/" + std::to_string(AID) +
                                  "/START_" + std::to_string(AID) + ".txt";

    std::ofstream file(start_text_path);
    if (!file.is_open()) {
        std::filesystem::remove_all(AID_dirname);
        return 1;
    }

    file << std::to_string(start_value);
    file.close();
    return 0;
}

int endAuction(int AID) {
    std::string end_text_path = "AUCTIONS/" + std::to_string(AID) + "/END_" +
                                std::to_string(AID) + ".txt";

    std::ofstream file(end_text_path);
    if (!file.is_open()) {
        return 1;
    }

    file.close();
    return 0;
}

int addBidToAuction(int AID, std::string user_that_bid, int bid_value) {
    std::string bid_text_path = "AUCTIONS/" + std::to_string(AID) + "/BIDS/" +
                                std::to_string(bid_value) + ".txt";

    std::ofstream file(bid_text_path);
    if (!file.is_open()) {
        return 1;
    }

    file << user_that_bid;
    file.close();

    // Write in start text of auction the highest bid, deletes previous value
    // written in the start text
    std::string start_text_path = "AUCTIONS/" + std::to_string(AID) +
                                  "/START_" + std::to_string(AID) + ".txt";

    std::ofstream start_text_file(start_text_path, std::ios::trunc);
    if (!start_text_file.is_open()) {
        return 1;
    }

    start_text_file << bid_value;
    start_text_file.close();
    return 0;
}

bool checkIfAuctionExists(int AID) {
    std::string auction_dir = "AUCTIONS/" + std::to_string(AID);
    return std::filesystem::exists(auction_dir);
}

bool checkIfAuctionIsActive(int AID) {
    std::string end_file = "AUCTIONS/" + std::to_string(AID) + "/END_" + std::to_string(AID) + ".txt";
    return !std::filesystem::exists(end_file);
}

bool isNewBidHigher(int AID, int bid_value) {
    std::string start_text_path = "AUCTIONS/" + std::to_string(AID) +
                                  "/START_" + std::to_string(AID) + ".txt";

    std::ifstream start_text_file(start_text_path);
    if (!start_text_file.is_open()) {
        return false;
    }

    int current_bid_value;
    start_text_file >> current_bid_value;

    start_text_file.close();
    return bid_value > current_bid_value;
}

