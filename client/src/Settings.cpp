#include "Settings.h"

// Util functions

// estabilish conection with the server
bool Client::connectToServer(const std::string &ipAddress, const std::string &port, const std::string &username,
                             const std::string &password) {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    struct addrinfo *result = NULL, *ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = getaddrinfo(ipAddress.c_str(), port.c_str(), &hints, &result);
    if (iResult != 0) {
        std::cerr << "getaddrinfo failed: " << iResult << "\n";
        WSACleanup();
        return false;
    }

    ptr = result;
    _connection_socket = socket(ptr->ai_family, ptr->ai_socktype, ptr->ai_protocol);

    if (_connection_socket == INVALID_SOCKET) {
        std::cerr << "Error at socket(): " <<  WSAGetLastError() << "\n";
        freeaddrinfo(result);
        WSACleanup();
        return false;
    }

    iResult = connect(_connection_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server!\n";
        closesocket(_connection_socket);
        _connection_socket = INVALID_SOCKET;
        return false;
    }

    freeaddrinfo(result);

    // Przesłanie danych logowania
    std::string loginData = "LOGIN " + username + " " + password;
    iResult = send(_connection_socket, loginData.c_str(), loginData.length(), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(_connection_socket);
        WSACleanup();
        return false;
    }

    std::cout << "Login data sent successfully.\n";

    // wait for response from server
    char recvbuf[DEFAULT_BUFLEN];
    iResult = recv(_connection_socket, recvbuf, DEFAULT_BUFLEN, 0);
    if (iResult > 0) {
        recvbuf[iResult] = '\0';
        std::cout << "Server response: " << recvbuf << "\n";
        if (strcmp(recvbuf, "LOGIN_SUCCESS") != 0) {
            std::cerr << "Login failed.\n";
            closesocket(_connection_socket);
            WSACleanup();
            return false;
        }
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << "\n";
        closesocket(_connection_socket);
        WSACleanup();
        return false;
    }

    return true;
}


// sending message to server
void Client::sendMessage(const std::string &message) {
    int iResult = send(_connection_socket, message.c_str(), message.length(), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(_connection_socket);
        WSACleanup();
    }
}