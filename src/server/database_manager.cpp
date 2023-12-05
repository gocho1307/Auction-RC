#include "database_manager.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"

/*
int createUserDir(std::string UID, std::string password) {

    return 0;
}
*/

int createAuctionDir(int AID) {
    char AID_dirname[15];
    char BIDS_dirname[20];
    int ret;

    if (AID < MIN_AID || AID > MAX_AID) {
        return 1;
    }

    // Create the AUCTIONS directory
    sprintf(AID_dirname, "AUCTIONS/%03d", AID);
    ret = mkdir(AID_dirname, 0700);

    if (ret == -1) {
        std::cerr << CREATE_DIR_ERR << AID_dirname << std::endl;
        return 1;
    }

    // Create the BIDS directory inside AUCTIONS
    sprintf(BIDS_dirname, "AUCTIONS/%03d/BIDS", AID);
    ret = mkdir(BIDS_dirname, 0700);

    if (ret == -1) {
        std::cerr << CREATE_DIR_ERR << BIDS_dirname << std::endl;
        rmdir(AID_dirname);
        return 1;
    }

    return 0;
}