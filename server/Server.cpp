#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

// Internal headers
#include "Settings.h"

int main(int argc, char* argv[]) {

    Server server;
    server.runServer();

    std::cin.get();
    return 0;
}