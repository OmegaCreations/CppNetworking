#ifndef SERVER_SETTINGS_H
#define SERVER_SETTINGS_H
#pragma once

// External libs
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <iostream>
#include <string>
#include <map>
#include <random>
#include <thread>

// Internal libs
#include "SessionManager.h"

// Defaults
#define DEFAULT_PORT "27015"
#define DEFAULT_BUFLEN 512


// Main server class featuring all server configuration and reusable actions
class Server {
private:

    // config
    int _is_running = 1;
    WSADATA _wsa_data{}; // WSADATA
    int _response_result; // response provided by winsock2 actions
    std::string _port; // server running port
    int _bufflen; // default buffer length to process
    struct addrinfo* _addrinfo_result = nullptr, *ptr = nullptr, hints{};

    // sockets
    SOCKET _listener_socket{}; // listener socket

    // session handling
    static std::string processRequest(const std::string& request);


public:
    // Constructors
    Server(const char *port);
    Server();
    ~Server();

    // Util methods
    void runServer();

    // clear all connections
    void serverCleanup();

    // initialize listener socket and bind it
    void openListener();
    // stop server
    void stopServer();

    // handle client connection
    void handleConnection(SOCKET client_socket);
    SOCKET acceptConnection();

    // handle common winsocket2 errors
    int check(int i);
    int check(SOCKET socket);
};


#endif //SERVER_SETTINGS_H
