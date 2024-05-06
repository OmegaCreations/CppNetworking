#ifndef SERVER_SESSIONMANAGER_H
#define SERVER_SESSIONMANAGER_H
#pragma once

// External libs
#include <winsock2.h>
#include <windows.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <unordered_map>
#include <iostream>
#include <string>
#include <ctime>
#include <random>
#include <chrono>

class SessionManager {
private:
    // user's session
    struct SessionData {
        std::string username; // username
        std::chrono::steady_clock::time_point lastActiveTime; // time of last activity
        SOCKET* client_socket;
        std::string chat_id; // current chat room id
    };

    static constexpr int sessionTimeoutMinutes = 30; // session timeout

public:
    // token, Session data
    static std::unordered_map<std::string, SessionData> sessions; // contains users' sessions

    // generate unique token
    static std::string generateSessionToken();

    // start new user's session
    static std::string startSession(const std::string& username, SOCKET client_socket);

    // verify existing session token
    static bool verifySessionToken(const std::string& token);
};

#endif //SERVER_SESSIONMANAGER_H
