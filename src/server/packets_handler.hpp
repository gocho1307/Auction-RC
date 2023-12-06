#ifndef __PACKETS_HANDLER_HPP__
#define __PACKETS_HANDLER_HPP__

#include "server_state.hpp"

// UDP

void handleLogin(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleLogout(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleUnregister(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleMyAuctions(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleMyBids(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleList(std::string &buffer, struct addrinfo *connection_addr, ServerState state);
void handleShowRecord(std::string &buffer, struct addrinfo *connection_addr, ServerState state);

// TCP

void handleOpenAuction(int connection_fd);
void handleCloseAuction(int connection_fd);
void handleBid(int connection_fd);
void handleShowAsset(int connection_fd);

#endif // __PACKETS_HANDLER_HPP__
