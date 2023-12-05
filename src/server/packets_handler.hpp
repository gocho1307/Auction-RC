#ifndef __PACKETS_HANDLER_HPP__
#define __PACKETS_HANDLER_HPP__

#include "server_state.hpp"

// UDP

void handleLogin(std::string &buffer, Address &addr_from);
void handleLogout(std::string &buffer, Address &addr_from);
void handleRegister(std::string &buffer, Address &addr_from);
void handleMyAuctions(std::string &buffer, Address &addr_from);
void handleMyBids(std::string &buffer, Address &addr_from);
void handleList(std::string &buffer, Address &addr_from);
void handleShowRecord(std::string &buffer, Address &addr_from);

// TCP

void handleOpenAuction(int connection_fd);
void handleCloseAuction(int connection_fd);
void handleBid(int connection_fd);
void handleShowAsset(int connection_fd);

#endif // __PACKETS_HANDLER_HPP__
