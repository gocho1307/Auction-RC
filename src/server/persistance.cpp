#include "persistance.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

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
    std::getline(passwordFile, storedPassword);
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
    passwordFile << password << std::endl;
    passwordFile.close();
    return loginUser(UID);
}

int loginUser(std::string UID) {
    std::string loginPath = "USERS/" + UID + "/login";
    std::ofstream loginFile(loginPath);
    if (!loginFile.is_open()) {
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

void getAuctionTime(std::string AID, uint32_t &fullTime, uint32_t &duration) {
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
}

int checkAuctionExpiration(std::string AID, time_t &currentTime) {
    if (std::filesystem::exists("AUCTIONS/" + AID + "/end.txt")) {
        return 0;
    }

    currentTime = time(NULL); // TODO: must be positive
    uint32_t fullTime, duration;
    getAuctionTime(AID, fullTime, duration);

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

uint8_t getAuctionState(std::string AID) {
    time_t t; // not used
    return (uint8_t)checkAuctionExpiration(AID, t);
}

int getAuctions(std::string path, std::vector<Auction> &auctions) {
    const std::filesystem::path auctionsPath(path);
    int activeAuctions = 0;

    for (auto const &entry :
         std::filesystem::directory_iterator(auctionsPath)) {
        std::string AID = entry.path().filename();
        uint8_t state = getAuctionState(AID);
        if (state) {
            activeAuctions++;
        }
        auctions.push_back({AID, state});
    }
    return (activeAuctions != 0);
}

int closeAuction(std::string AID) {
    time_t currentTime;
    if (checkAuctionExpiration(AID, currentTime)) {
        return 0;
    }

    std::ofstream endFile("AUCTIONS/" + AID + "/end.txt");
    if (!endFile.is_open()) {
        return 0;
    }
    uint32_t fullTime, duration;
    getAuctionTime(AID, fullTime, duration);
    endFile << toDate(currentTime) << " " << (uint32_t)currentTime - fullTime
            << std::endl;
    endFile.close();
    return 1;
}

int checkAuctionExists(std::string AID) {
    return std::filesystem::exists("AUCTIONS/" + AID);
}

int checkUserHostedAuction(std::string UID, std::string AID) {
    return std::filesystem::exists("USERS/" + UID + "HOSTED/" + AID);
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
    std::ofstream nameFile(auctionDir + "/ASSET/name.txt");
    std::ofstream highestBidFile(auctionDir + "/BIDS/highest.txt");
    std::ofstream bidsFile(auctionDir + "/BIDS/list.txt");
    std::ofstream hostFile("USERS/" + UID + "/HOSTED/" + newAID);
    if (!startFile.is_open() || !timeFile.is_open() || !nameFile ||
        !highestBidFile.is_open() || !bidsFile.is_open() ||
        !hostFile.is_open()) {
        std::filesystem::remove_all(auctionDir);
        return 0;
    }
    bidsFile.close();
    hostFile.close();
    try {
        std::filesystem::rename(assetfName,
                                auctionDir + "/ASSET/" + assetfName);
    } catch (std::filesystem::filesystem_error &e) {
        std::filesystem::remove_all(auctionDir);
        return 0;
    }

    nameFile << assetfName << std::endl;
    nameFile.close();
    highestBidFile << startValue << std::endl;
    highestBidFile.close();
    time_t startTime = time(NULL); // TODO: must be positive
    timeFile << startTime << std::endl << duration << std::endl;
    timeFile.close();
    startFile << UID << " " << auctionName << " " << assetfName << " "
              << startValue << " " << toDate(startTime) << " " << duration
              << std::endl;
    startFile.close();

    return 1;
}

int getAuctionRecord(std::string AID, std::string &info) {
    if (!checkAuctionExists(AID)) {
        return 0;
    }

    std::ifstream startFile("AUCTIONS/" + AID + "/start.txt");
    std::ifstream bidsFile("AUCTIONS/" + AID + "/BIDS/list.txt");
    if (!startFile.is_open() || !bidsFile.is_open()) {
        return 0;
    }
    std::getline(startFile, info);
    startFile.close();

    std::string bidInfo;
    int i = 0;
    while (std::getline(bidsFile, bidInfo) && i++ < MAX_BIDS_LISTINGS) {
        info += " B " + bidInfo;
    }

    if (getAuctionState(AID)) {
        std::ifstream endFile("AUCTIONS/" + AID + "/end.txt");
        if (!endFile.is_open()) {
            return 0;
        }
        std::string endInfo;
        std::getline(endFile, endInfo);
        info += " E " + endInfo;
        endFile.close();
    }

    return 1;
}

int bidAuction(std::string AID, std::string UID, uint32_t value,
               time_t currentTime) {
    std::ifstream highestFile("AUCTIONS/" + AID + "/BIDS/highest.txt");
    if (!highestFile.is_open()) {
        return 0;
    }
    std::string strHighestValue;
    uint32_t highestValue;
    std::getline(highestFile, strHighestValue);
    toInt(strHighestValue, highestValue);
    highestFile.close();
    if (value <= highestValue) {
        return 0;
    }

    std::ofstream highFile("AUCTIONS/" + AID + "/BIDS/highest.txt");
    std::ofstream bidsFile("AUCTIONS/" + AID + "/BIDS/list.txt", std::ios::app);
    if (!highFile.is_open() || !bidsFile.is_open()) {
        return 0;
    }
    highFile << value << std::endl;
    highFile.close();
    uint32_t fullTime, duration;
    getAuctionTime(AID, fullTime, duration);
    bidsFile << UID << " " << value << " " << toDate(currentTime) << " "
             << (uint32_t)currentTime - fullTime << std::endl;
    bidsFile.close();

    std::ofstream userBidFile("USERS/" + UID + "/BIDDED/" + AID);
    if (!userBidFile.is_open()) {
        return 0;
    }
    userBidFile.close();

    return 1;
}

int getAuctionAsset(std::string AID, std::string &fPath) {
    std::ifstream nameFile("AUCTIONS/" + AID + "/ASSET/name.txt");
    if (!nameFile.is_open()) {
        return 0;
    }
    std::getline(nameFile, fPath);
    nameFile.close();
    return 1;
}
