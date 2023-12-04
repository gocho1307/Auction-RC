#include "server.hpp"
#include "../lib/constants.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"
#include "server_state.hpp"

#include <netdb.h>
#include <thread>

ServerState state;

int main(int argc, char *argv[]) {
    state.readOpts(argc, argv);

    if (state.verbose_mode) {
        std::cout << "Verbose mode is on." << std::endl;
    }

    state.registerPacketHandlers();

    // TO DO
    // std::thread tcp_thread(tcp, std::ref(state));

    return EXIT_SUCCESS;
}

void tcp() {
    // TO DO
    if (listen(state.socketTCP, MAX_TCP_QUEUE) < 0) {
        std::cerr << TCP_LISTEN_ERR << std::endl;
        state.shutDown = true;
        return;
    }
}

void printHelp(std::ostream &stream, char *programPath) {
    stream << "Usage: " << programPath << "[-p ASport] [-v]" << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default is: "
           << DEFAULT_AS_PORT << std::endl;
    stream << "-v\t\tTo run the program in verbose mode." << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
}