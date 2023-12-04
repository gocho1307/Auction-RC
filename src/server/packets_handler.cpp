#include "packets_handler.hpp"

// UDP

void handle_login(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}

void handle_logout(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}

void handle_register(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}
void handle_my_auctions(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}
void handle_my_bids(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}
void handle_list(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}
void handle_show_record(std::string &buffer, Address &addr_from) {
    (void)buffer;
    (void)addr_from;
    // TO DO
}

// TCP

void handle_open_bid(int connection_fd) {
    (void)connection_fd;
    // TO DO
}
void handle_close_bid(int connection_fd) {
    (void)connection_fd;
    // TO DO
}
void handle_bid(int connection_fd) {
    (void)connection_fd;
    // TO DO
}
void handle_show_asset(int connection_fd) {
    (void)connection_fd;
    // TO DO
}
