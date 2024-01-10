#include "packets.hpp"
#include "../lib/messages.hpp"
#include "../lib/protocol.hpp"
#include "persistance.hpp"
#include "server_state.hpp"

#include <iomanip>
#include <sstream>
#include <unistd.h>

UDPPacketsHandler UDPHandler = {{"LIN ", LINHandler}, {"LOU ", LOUHandler},
                                {"UNR ", UNRHandler}, {"LMA ", LMAHandler},
                                {"LMB ", LMBHandler}, {"LST\n", LSTHandler},
                                {"SRC ", SRCHandler}};
TCPPacketsHandler TCPHandler = {{"OPA ", OPAHandler},
                                {"CLS ", CLSHandler},
                                {"SAS ", SASHandler},
                                {"BID ", BIDHandler}};

void interpretUDPPacket(ServerState &state, std::string msg, Address UDPFrom) {
    std::string packetID = msg.substr(0, PACKET_ID_LEN + 1);
    msg.erase(0, PACKET_ID_LEN + 1);

    if (UDPHandler.find(packetID) == UDPHandler.end()) {
        ERRUDPPacket err;
        sendUDPPacket(err, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                      state.socketUDP);
        state.cverbose << "| " << UNKNOWN_MSG << std::endl;
        return;
    }
    UDPHandler[packetID](state, msg, UDPFrom);
}

void interpretTCPPacket(ServerState &state, const int fd) {
    char c;
    std::string packetID;
    int n = PACKET_ID_LEN + 1;
    while (n-- > 0 && read(fd, &c, 1) == 1) {
        packetID.push_back(c);
    }

    if (packetID.length() != PACKET_ID_LEN + 1 ||
        TCPHandler.find(packetID) == TCPHandler.end()) {
        ERRTCPPacket err;
        err.serialize(fd);
        state.cverbose << "| " << UNKNOWN_MSG << std::endl;
        return;
    }
    TCPHandler[packetID](state, fd);
}

void LINHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LINPacket packetIn;
    RLIPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to login with the password '"
                       << packetIn.password << "'" << std::endl;

        if (checkRegister(packetIn.UID)) {
            if (checkLoginMatch(packetIn.UID, packetIn.password) &&
                loginUser(packetIn.UID)) {
                packetOut.status = "OK";
            } else {
                packetOut.status = "NOK";
            }
        } else if (registerUser(packetIn.UID, packetIn.password)) {
            packetOut.status = "REG";
        } else {
            packetOut.status = "ERR";
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LOUHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LOUPacket packetIn;
    RLOPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to logout" << std::endl;

        if (!checkRegister(packetIn.UID)) {
            packetOut.status = "UNR";
        } else if (!checkLoggedIn(packetIn.UID) ||
                   !checkLoginMatch(packetIn.UID, packetIn.password) ||
                   !logoutUser(packetIn.UID)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void UNRHandler(ServerState &state, std::string msg, Address UDPFrom) {
    UNRPacket packetIn;
    RURPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to unregister" << std::endl;

        if (!checkRegister(packetIn.UID)) {
            packetOut.status = "UNR";
        } else if (!checkLoggedIn(packetIn.UID) ||
                   !checkLoginMatch(packetIn.UID, packetIn.password) ||
                   !unregisterUser(packetIn.UID)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LMAHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMAPacket packetIn;
    RMAPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to list their hosted auctions" << std::endl;

        std::vector<Auction> auctions;
        if (!checkLoggedIn(packetIn.UID)) {
            packetOut.status = "NLG";
        } else if (!getAuctions("USERS/" + packetIn.UID + "/HOSTED",
                                auctions)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.auctions = auctions;
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LMBHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LMBPacket packetIn;
    RMBPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose
            << "| User with id '" << packetIn.UID
            << "' asked to list the auctions in which they have bidded"
            << std::endl;

        std::vector<Auction> auctions;
        if (!checkLoggedIn(packetIn.UID)) {
            packetOut.status = "NLG";
        } else if (!getAuctions("USERS/" + packetIn.UID + "/BIDDED",
                                auctions)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.auctions = auctions;
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void LSTHandler(ServerState &state, std::string msg, Address UDPFrom) {
    LSTPacket packetIn;
    RLSPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| A user asked to list all of the auctions"
                       << std::endl;

        std::vector<Auction> auctions;
        if (!getAuctions("AUCTIONS", auctions)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.auctions = auctions;
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void SRCHandler(ServerState &state, std::string msg, Address UDPFrom) {
    SRCPacket packetIn;
    RRCPacket packetOut;

    if (packetIn.deserialize(msg)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| A user asked for the record of auction number '"
                       << packetIn.AID << "'" << std::endl;

        std::string info;
        if (!getAuctionRecord(packetIn.AID, info)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.info = info;
        }
    }
    sendUDPPacket(packetOut, (struct sockaddr *)&UDPFrom.addr, UDPFrom.addrlen,
                  state.socketUDP);
}

void OPAHandler(ServerState &state, int fd) {
    OPAPacket packetIn;
    ROAPacket packetOut;

    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to create an auction with name '"
                       << packetIn.auctionName << "', a start value of '"
                       << packetIn.startValue << "' and a maximum duration of '"
                       << packetIn.duration << "' seconds" << std::endl;

        std::string newAID = getNewAID();
        if (!checkLoggedIn(packetIn.UID) ||
            !checkLoginMatch(packetIn.UID, packetIn.password)) {
            packetOut.status = "NLG";
        } else if (!openAuction(newAID, packetIn.UID, packetIn.auctionName,
                                packetIn.assetfName, packetIn.startValue,
                                packetIn.duration)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.AID = newAID;
        }
    }
    packetOut.serialize(fd);
}

void CLSHandler(ServerState &state, const int fd) {
    CLSPacket packetIn;
    RCLPacket packetOut;

    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to close the auction number '"
                       << packetIn.AID << "'" << std::endl;

        if (!checkLoggedIn(packetIn.UID) ||
            !checkLoginMatch(packetIn.UID, packetIn.password)) {
            packetOut.status = "NLG";
        } else if (!checkAuctionExists(packetIn.AID)) {
            packetOut.status = "EAU";
        } else if (!checkUserHostedAuction(packetIn.UID, packetIn.AID)) {
            packetOut.status = "EOW";
        } else if (!closeAuction(packetIn.AID)) {
            packetOut.status = "END";
        } else {
            packetOut.status = "OK";
        }
    }
    packetOut.serialize(fd);
}

void BIDHandler(ServerState &state, const int fd) {
    BIDPacket packetIn;
    RBDPacket packetOut;

    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| User with id '" << packetIn.UID
                       << "' asked to place a bid of '" << packetIn.value
                       << "' in auction number '" << packetIn.AID << "'"
                       << std::endl;

        time_t currentTime;
        if (!checkLoggedIn(packetIn.UID) ||
            !checkLoginMatch(packetIn.UID, packetIn.password)) {
            packetOut.status = "NLG";
        } else if (!checkAuctionExpiration(packetIn.AID, currentTime)) {
            packetOut.status = "NOK";
        } else if (checkUserHostedAuction(packetIn.UID, packetIn.AID)) {
            packetOut.status = "ILG";
        } else if (!bidAuction(packetIn.AID, packetIn.UID, packetIn.value,
                               currentTime)) {
            packetOut.status = "REF";
        } else {
            packetOut.status = "ACC";
        }
    }
    packetOut.serialize(fd);
}

void SASHandler(ServerState &state, const int fd) {
    SASPacket packetIn;
    RSAPacket packetOut;

    if (packetIn.deserialize(fd)) {
        packetOut.status = "ERR";
    } else {
        state.cverbose << "| A User asked for the asset of auction number '"
                       << packetIn.AID << "'" << std::endl;

        std::string fPath;
        if (!getAuctionAsset(packetIn.AID, fPath)) {
            packetOut.status = "NOK";
        } else {
            packetOut.status = "OK";
            packetOut.assetfPath = fPath;
        }
    }
    packetOut.serialize(fd);
}
