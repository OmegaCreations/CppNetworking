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

#define DEFAULT_PORT "27015"
#define DEFAULT_ADDR "127.0.0.1"
#define DEFAULT_BUFLEN 512

class Client {
private:
    WSADATA wsaData;
    SOCKET _connection_socket;
public:
    // Constructors
    Client() : _connection_socket(INVALID_SOCKET) {}
    ~Client() {
        closesocket(_connection_socket);
        WSACleanup();
    }

    // Util functions
    // Funkcja łącząca z serwerem i wysyłająca dane logowania
    bool connectToServer(const std::string& ipAddress, const std::string& port, const std::string& username, const std::string& password);

    // Funkcja wysyłająca wiadomość do serwera
    void sendMessage(const std::string& message);


};



#endif //CLIENT_SETTINGS_H
