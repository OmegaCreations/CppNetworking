#ifndef CLIENT_SETTINGS_H
#define CLIENT_SETTINGS_H
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
#include <sstream>
#include <vector>

// internal files
#include "Utils.h"

#define DEFAULT_PORT "27015"
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_BUFLEN 512

class Client {
private:
    WSADATA wsaData;
    SOCKET _connection_socket;
    SOCKET _listener_socket;

    std::string _username;
    std::string _token; // login token
    std::string _room_id;
    std::string _active_rooms;
    bool _is_logged_in;

    std::thread consoleInputThread;

public:

    // Constructors
    Client() : _connection_socket(INVALID_SOCKET), _room_id(""), _is_logged_in(false) { }
    ~Client() {
        closesocket(_connection_socket);
        WSACleanup();
    }

    // Util functions
    std::string getChatToken() const;


    // Funkcja ³¹cz¹ca z serwerem i wysy³aj¹ca dane logowania
    bool connectToServer(const std::string& ipAddress, const std::string& port, const std::string& username, const std::string& password);
    bool connectToServerWithToken();

    // Funkcja wysy³aj¹ca wiadomoœæ do serwera
    void sendMessage(const std::string& message);
    std::vector<std::string> getServerChatMessage();

    // Log In handling
    void logIn(std::string username, std::string password);

    // Requests handler
    std::vector<std::string> getAllRooms();
    void joinRoom(std::string room_id);
    void exitRoom(std::string room_id);

    // response handling
    void responseHandler(SOCKET socket);

    // User input handling
    void handleInput(std::string input);
};


#endif //CLIENT_SETTINGS_H
