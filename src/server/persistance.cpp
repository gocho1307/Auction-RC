#include "persistance.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <filesystem>
#include <fstream>

int checkRegister(const std::string UID) {
    return std::filesystem::exists("USERS/" + UID) &&
           std::filesystem::exists("USERS/" + UID + "/password.txt");
}

int checkLoggedIn(std::string UID) {
    return checkRegister(UID) &&
           std::filesystem::exists("USERS/" + UID + "/login");
}

int checkLoginMatch(std::string UID, std::string password) {
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
    std::string loginPath = "USERS/" + UID + "/login";
    std::ofstream loginFile(loginPath);
    if (!std::filesystem::exists(loginPath)) {
        std::cerr << FILE_CREATE_ERR(loginPath) << std::endl;
        return 0;
    }
    loginFile.close();
    return 1;
}

int logoutUser(std::string UID) {
    return std::filesystem::remove("USERS/" + UID + "/login");
}

int unregisterUser(std::string UID) {
    return logoutUser(UID) &&
           std::filesystem::remove("USERS/" + UID + "/password.txt");
}

void getAuctionTime(std::string AID, time_t &currentTime, uint32_t &fullTime,
                    uint32_t &duration) {
    std::ifstream timeFile("AUCTIONS/" + AID + "/time.txt");
    if (!timeFile.is_open()) {
        // TODO: throw exception
    }
    std::string strFullTime, strDuration;
    std::getline(timeFile, strFullTime);
    std::getline(timeFile, strDuration);
    toInt(strFullTime, fullTime);
    toInt(strDuration, duration);
    timeFile.close();
    currentTime = time(NULL); // TODO: must be positive
}

uint8_t checkAuctionExpiration(std::string AID) {
    if (std::filesystem::exists("AUCTIONS/" + AID + "/end.txt")) {
        return 0;
    }

    time_t currentTime;
    uint32_t fullTime, duration;
    getAuctionTime(AID, currentTime, fullTime, duration);

    if ((uint32_t)currentTime - fullTime >= duration) {
        std::ofstream endFile("AUCTIONS/" + AID + "/end.txt");
        if (!endFile.is_open()) {
            return 0;
        }
        endFile << toDate(fullTime + duration) << " " << duration << std::endl;
        endFile.close();
        return 0;
    }
    return 1;
}

int getAuctions(std::string path, std::vector<Auction> &auctions) {
    const std::filesystem::path auctionsPath(path);
    int activeAuctions = 0;

    for (auto const &entry :
         std::filesystem::directory_iterator(auctionsPath)) {
        std::string AID = entry.path().filename();
        uint8_t state = checkAuctionExpiration(AID);
        if (state) {
            activeAuctions++;
        }
        auctions.push_back({AID, state});
    }
    return (activeAuctions != 0);
}

int closeAuction(std::string AID) {
    if (std::filesystem::exists("AUCTIONS/" + AID + "/end.txt")) {
        return 0;
    }
    std::ofstream endFile("AUCTIONS/" + AID + "/end.txt");
    if (!endFile.is_open()) {
        return 0;
    }

    time_t currentTime;
    uint32_t fullTime, duration;
    getAuctionTime(AID, currentTime, fullTime, duration);

    uint32_t finishTime = (uint32_t)currentTime - fullTime;
    if (finishTime >= duration) {
        endFile << toDate(fullTime + duration) << " " << duration << std::endl;
        endFile.close();
        return 0;
    }
    endFile << toDate(currentTime) << " " << finishTime << std::endl;
    endFile.close();
    return 1;
}

int checkAuctionExists(std::string AID) {
    return std::filesystem::exists("AUCTIONS/" + AID);
}

int checkUserHostedAuction(std::string UID, std::string AID) {
    return std::filesystem::exists("USERS/" + UID + "HOSTED/" + AID);
}

int openAuction(std::string newAID, std::string UID, std::string auctionName,
                std::string assetfName, uint32_t startValue,
                uint32_t duration) {
    if (newAID.empty()) {
        return 0; // reached the maximum number of auctions
    }
    std::string auctionDir = "AUCTIONS/" + newAID;
    if (!std::filesystem::create_directory(auctionDir)) {
        return 0;
    }
    if (!std::filesystem::create_directory(auctionDir + "/ASSET") ||
        !std::filesystem::create_directory(auctionDir + "/BIDS")) {
        std::filesystem::remove_all(auctionDir);
        return 0;
    }
    std::ofstream startFile(auctionDir + "/start.txt");
    std::ofstream timeFile(auctionDir + "/time.txt");
    if (!startFile.is_open() || !timeFile.is_open()) {
        std::filesystem::remove_all(auctionDir);
        return 0;
    }
    try {
        std::filesystem::rename(assetfName,
                                auctionDir + "/ASSET/" + assetfName);
    } catch (std::filesystem::filesystem_error &e) {
        std::filesystem::remove_all(auctionDir);
        return 0;
    }

    time_t startTime = time(NULL); // TODO: must be positive
    std::string date = toDate(startTime);
    timeFile << startTime << std::endl << duration << std::endl;
    timeFile.close();
    startFile << UID << " " << auctionName << " " << assetfName << " "
              << startValue << " " << date << " " << duration << std::endl;
    startFile.close();

    return 1;
}

std::string getNewAID() {
    ssize_t count =
        std::distance(std::filesystem::directory_iterator("AUCTIONS"),
                      std::filesystem::directory_iterator{});
    if (count >= MAX_AUCTIONS) {
        return "";
    }

    std::stringstream ss;
    ss << std::setfill('0') << std::setw(AID_LEN) << count + 1;
    return ss.str();
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
