#ifndef __MESSAGES_HPP__
#define __MESSAGES_HPP__

#include "constants.hpp"

#define DEFAULT_AS_HOST_STR                                                    \
    "[INFO] Could not find ASIP address, using default host: "                 \
        << DEFAULT_AS_HOST
#define DEFAULT_AS_PORT_STR                                                    \
    "[INFO] Could not find ASport number, using default port: "                \
        << DEFAULT_AS_PORT
#define SHUTDOWN_STR "Closing the user application..."

#define GETADDRINFO_UDP_ERR "[ERR] Failed to get address for UDP connection: "
#define GETADDRINFO_TCP_ERR "[ERR] Failed to get address for TCP connection: "
#define SOCKET_CREATE_ERR "[ERR] Failed to create socket."
#define SOCKET_CLOSE_ERR "[ERR] Failed to close socket."
#define SOCKET_TIMEOUT_ERR "[ERR] Failed to set socket timeout."
#define TCP_CONNECT_ERR "[ERR] Failed to establish TCP connection."
#define PORT_ERR "[ERR] Invalid port number."
#define READING_ERR "[ERR] An unexpected error occured while reading a string."
#define SIGACTION_ERR "[ERR] Failed to set signal action."
#define SENDTO_ERR "[ERR] Failed to send message via UDP."
#define RECVFROM_ERR "[ERR] Failed to receive message via UDP."
#define WRITE_ERR "[ERR] Failed to send message via TCP."
#define READ_ERR "[ERR] Failed to receive message via TCP."
#define FILE_ERR "[ERR] Failed to process the file."

#define UNEXPECTED_COMMAND_ERR(commandName)                                    \
    "[ERR] The command '" << commandName                                       \
                          << "' is not supported by this application."
#define PACKET_ERR "[ERR] An error occured with the packet. Please try again."
#define STATUS_ERR "[ERR] Received invalid status in message. Please try again."
#define FILE_NAME_ERR                                                          \
    "[ERR] The filename must only contain alphanumeric characters, '.', '-' "  \
    "and '_'."
#define FILE_SIZE_ERR "[ERR] Incorrect file size received for the data."
#define LOGIN_ERR                                                              \
    "A user is already logged in. To log into another account, please log "    \
    "out first."
#define UID_ERR "Invalid user ID. Expected a 6 digit positive number."
#define PASSWORD_ERR "Invalid password. Expected 8 alphanumeric characters."
#define LOGIN_OK "Logged in successfully."
#define LOGIN_NOK(user) "Incorrect password for user '" << user << "'."
#define LOGIN_REG(user) "New user '" << user << "' registered and logged in."
#define LOGOUT_OK "Logged out successfully."
#define UNREG_OK(user) "Successfully unregistered the user '" << user << "'."
#define NO_LOGIN "No user is currently logged in."
#define NOT_LOGGED "User is not logged in."
#define NO_REG(user) "User '" << user << "' is not registered."
#define EXIT_ERR                                                               \
    "A user is still logged in, please make sure to logout before exitting "   \
    "the application."
#define NAME_ERR                                                               \
    "Invalid description. Expected up to 10 alphanumeric characters."
#define START_VAL_ERR                                                          \
    "Invalid start value for the auction. Expected a"                          \
    " positive value with up to 6 digits."
#define DURATION_ERR                                                           \
    "Invalid duration (in seconds) for the auction. Expected a"                \
    " positive value with up to 5 digits."
#define OPEN_OK(aid) "New auction with the id of:" << aid << "was created."
#define OPEN_NOK "Could not open the auction."
#define AID_ERR                                                                \
    "Invalid auction id. Expected a positive value with up to"                 \
    " 3 digits."
#define CLOSE_OK "Closed the auction successfully."
#define CLOSE_EAU "The auction you tried to close does not exist."
#define CLOSE_EOW "Cannot close auctions you do not own."
#define CLOSE_END "This auction was already closed."
#define MYAUCTIONS_OK "Here are your ongoing auctions"
#define MYAUCTIONS_NOK "You do not have any ongoing auctions."
#define MYBIDS_OK "Here are the ongoing auctions in which you have placed a bid"
#define MYBIDS_NOK "You do not have any ongoing bids."
#define LIST_OK "Here are all the ongoing auctions"
#define LIST_NOK "There are no auctions."
#define BID_NOK "The auction you tried to bid on is not active."
#define BID_ACC "Your bid was accepted."
#define BID_REF "Your bid was refused. A larger bid has already been placed."
#define BID_ILG "You cannot bid on an auction that you hosted."

#endif // __MESSAGES_HPP__
