#ifndef __MESSAGES_HPP__
#define __MESSAGES_HPP__

#include "constants.hpp"

#define DEFAULT_AS_HOST_STR                                                    \
    "[INFO] Could not find ASIP address, using default host: " << DEFAULT_AS_HOST
#define DEFAULT_AS_PORT_STR                                                    \
    "[INFO] Could not find ASport number, using default port: "                \
        << DEFAULT_AS_PORT

#define SHUTDOWN_STR "Closing the user application..."

#define GETADDRINFO_UDP_ERR "[ERR] Failed to get address for UDP connection: "
#define GETADDRINFO_TCP_ERR "[ERR] Failed to get address for TCP connection: "

#define PORT_ERR "[ERR] Invalid port number."

#define USAGE(command) "Error in command. Usage: " << command
#define UNEXPECTED_COMMAND_ERR(command)                                        \
    "[ERR]: The command '" << command                                          \
                           << "' is not supported by this application."

#define LOGIN_ERR "[ERR] User is already logged in."
#define UID_ERR "[ERR] Invalid user ID. Expected a 6 digit positive number."
#define PASSWORD_ERR                                                           \
    "[ERR] Invalid password. Expected 8 alphanumeric characters."
#define EXIT_ERR                                                               \
    "[ERR] Please make sure to logout before exitting the application."

#define READING_ERR "[ERR] An unexpected error occured while reading a string."

#endif // __MESSAGES_HPP__
