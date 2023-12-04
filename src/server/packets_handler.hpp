#ifndef __PACKETS_HANDLER_HPP__
#define __PACKETS_HANDLER_HPP__

#include "server_state.hpp"

// UDP

void handle_login(std::string &buffer, Address &addr_from);
void handle_logout(std::string &buffer, Address &addr_from);
void handle_register(std::string &buffer, Address &addr_from);
void handle_my_auctions(std::string &buffer, Address &addr_from);
void handle_my_bids(std::string &buffer, Address &addr_from);
void handle_list(std::string &buffer, Address &addr_from);
void handle_show_record(std::string &buffer, Address &addr_from);

// TCP

void handle_open_bid(int connection_fd);
void handle_close_bid(int connection_fd);
void handle_bid(int connection_fd);
void handle_show_asset(int connection_fd);

#endif // __PACKETS_HANDLER_HPP__
