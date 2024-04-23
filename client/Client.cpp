#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#include "Settings.h"

int main(int argc, char* argv[]) {
    if (argc != 5) {
        std::cerr << "Usage: " << argv[0] << " <server_ip> <port> <username> <password>\n";
        return 1;
    }

    Client client;
    if (!client.connectToServer(DEFAULT_ADDR, DEFAULT_PORT, "max", "passwd")) {
        std::cerr << "Failed to connect to the server.\n";
        return 1;
    }

    // Teraz klient jest połączony z serwerem i może wysyłać wiadomości
    client.sendMessage("Hello, server!");

    return 0;
}
