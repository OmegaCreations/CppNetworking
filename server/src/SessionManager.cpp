#include "SessionManager.h"
std::unordered_map<std::string, SessionManager::SessionData> SessionManager::sessions;


std::string SessionManager::generateSessionToken() {
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


std::string SessionManager::startSession(const std::string& username, SOCKET client_socket) {
    SOCKET* save_socket = new SOCKET;
    *save_socket = client_socket;

    std::string token = generateSessionToken();
    SessionData sessionData = {username, std::chrono::steady_clock::now(), save_socket, ""};
    sessions[token] = sessionData;
    return token;
}


bool SessionManager::verifySessionToken(const std::string& token) {
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
