#include "user.hpp"
#include "../lib/messages.hpp"
#include "../lib/utils.hpp"
#include "commands.hpp"
#include "user_state.hpp"

#include <netdb.h>

UserState state;

int main(int argc, char *argv[]) {
    setupSigHandlers(shutDownSigHandler);

    state.readOpts(argc, argv);
    checkPort(state.port);
    state.openUDPSocket();
    state.getServerAddresses();

    if (state.host.compare(DEFAULT_AS_HOST) == 0) {
        std::cout << DEFAULT_AS_HOST_STR << std::endl;
    }
    if (state.port.compare(DEFAULT_AS_PORT) == 0) {
        std::cout << DEFAULT_AS_PORT_STR << std::endl;
    }
    printTitle();
    helpCommand();

    while (!std::cin.eof() && !state.shutDown) {
        std::cout << PROMPT;
        std::getline(std::cin, state.line);
        interpretCommand(state);
    }

    std::cout << std::endl << SHUTDOWN_USER << std::endl;

    if (state.loggedIn) {
        logoutCommand(state);
    }
    return EXIT_SUCCESS;
}

void printTitle() {
    printf(
        "\n     _   _   _  ____ _____ ___ ___  _   _           ____   ____  \n"
        "    / \\ | | | |/ ___|_   _|_ _/ _ \\| \\ | |         |  _ \\ / ___| "
        "\n"
        "   / _ \\| | | | |     | |  | | | | |  \\| |  _____  | |_) | |     \n"
        "  / ___ \\ |_| | |___  | |  | | |_| | |\\  | |_____| |  _ <| |___  \n"
        " /_/   \\_\\___/ \\____| |_| |___\\___/|_| \\_|         |_| "
        "\\_\\\\____| \n");
}

void printHelp(std::ostream &stream, char *programPath) {
    stream << "Usage: " << programPath << " [-n ASIP] [-p ASport] [-h]"
           << std::endl;
    stream << "Available options:" << std::endl;
    stream << "-n ASIP\t\tSet hostname of Auction Server. Default is: "
           << DEFAULT_AS_HOST << std::endl;
    stream << "-p ASport\tSet port of Auction Server. Default is: "
           << DEFAULT_AS_PORT << std::endl;
    stream << "-h\t\tPrint this help menu." << std::endl;
}

void shutDownSigHandler(int sig) {
    (void)sig; // not used
    state.shutDown = true;
}
