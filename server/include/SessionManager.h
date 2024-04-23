#ifndef SERVER_SESSIONMANAGER_H
#define SERVER_SESSIONMANAGER_H
#pragma once

// External libs
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
    };

    static constexpr int sessionTimeoutMinutes = 30; // session timeout
    static std::unordered_map<std::string, SessionData> sessions; // contains users' sessions

public:
    // generate unique token
    static std::string generateSessionToken() {
        static const std::string charset = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ1234567890";

        // reference: https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
        static std::mt19937 gen(std::random_device{}());
        static std::uniform_int_distribution<int> dist(0, charset.length() - 1);

        constexpr int tokenLength = 32;
        std::string token;
        token.reserve(tokenLength);

        for (int i = 0; i < tokenLength; ++i) {
            token += charset[dist(gen)];
        }

        return token;
    }

    // start new user's session
    static std::string startSession(const std::string& username) {
        std::string token = generateSessionToken();
        SessionData sessionData = {username, std::chrono::steady_clock::now()};
        sessions[token] = sessionData;
        return token;
    }

    // verify existing session token
    static bool verifySessionToken(const std::string& token) {
        auto it = sessions.find(token);
        if (it != sessions.end()) {
            // check if session is still active
            auto currentTime = std::chrono::steady_clock::now();
            auto lastActiveTime = it->second.lastActiveTime;
            if (std::chrono::duration_cast<std::chrono::minutes>(currentTime - lastActiveTime).count() <= sessionTimeoutMinutes) {
                // refresh timeout
                it->second.lastActiveTime = currentTime;
                return true;
            } else {
                // remove session after timeout
                sessions.erase(it);
            }
        }
        return false;
    }
};

std::unordered_map<std::string, SessionManager::SessionData> SessionManager::sessions;


#endif //SERVER_SESSIONMANAGER_H
