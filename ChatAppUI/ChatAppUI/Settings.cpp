#include "Settings.h"

// Util functions
std::string Client::getChatToken() const {
    return _room_id;
}

// estabilish conection with the server
bool Client::connectToServer(const std::string& ipAddress, const std::string& port, const std::string& username,
    const std::string& password) {
    if (WSAStartup(MAKEWORD(2, 2), &wsaData) != 0) {
        std::cerr << "WSAStartup failed.\n";
        return false;
    }

    struct addrinfo* result = NULL, * ptr = NULL, hints;

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
        std::cerr << "Error at socket(): " << WSAGetLastError() << "\n";
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


    return true;
}

// connecting with token
bool Client::connectToServerWithToken() {

    struct addrinfo* result = NULL, * ptr = NULL, hints;

    ZeroMemory(&hints, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_protocol = IPPROTO_TCP;

    int iResult = connect(_connection_socket, ptr->ai_addr, (int)ptr->ai_addrlen);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "Unable to connect to server!\n";
        closesocket(_connection_socket);
        _connection_socket = INVALID_SOCKET;
        return false;
    }

    freeaddrinfo(result);


    return true;
}

// sending message to server
void Client::sendMessage(const std::string& message) {
    int iResult = send(_connection_socket, message.c_str(), message.length(), 0);
    if (iResult == SOCKET_ERROR) {
        std::cerr << "send failed: " << WSAGetLastError() << "\n";
        closesocket(_connection_socket);
        WSACleanup();
    }
}

// gets server message from listener socket
std::vector<std::string> Client::getServerChatMessage() {

    char recvbuf[DEFAULT_BUFLEN];
    int iResult = recv(_connection_socket, recvbuf, DEFAULT_BUFLEN, 0);
    std::vector<std::string> result;

    if (iResult > 0) {
        recvbuf[iResult] = '\0';

        std::istringstream iss((std::string)recvbuf);
        std::vector<std::string> data_set;
        std::string data;

        while (iss >> data) {
            data_set.push_back(data);
        }

        // LOGIN HANDLING
        if (data_set[0] == "LOGIN_SUCCESS") {

            _token = ((std::string)recvbuf).substr(14);
            std::cout << "Login successful.\n";
            return result;

        } // TOKEN VALID HANDLING
        else if (data_set[0] == "TOKEN_VALID") {
            std::cout << "Token confirmation successful.\n";
            return result;

        } // MESSAGE SENDING HANDLING
        else {
            result.push_back(data_set[0]);

            std::string message;
            std::for_each(data_set.begin() + 1, data_set.end(), [&](const std::string& req) {
                message.append(req + " ");
                });
            result.push_back(message);

            return result;
        }

    // Failed receiving response
    } else {
        std::cerr << "recv failed: " << WSAGetLastError() << "\n";
        return result;
    }
}

// handling logIn
void Client::logIn(std::string username, std::string password) {
    // Przes³anie danych logowania
    _username = username;
    std::string loginData = "LOGIN " + username + " " + password;
    sendMessage(loginData);

    std::cout << "Login data sent successfully.\n";
}

// Getting all active room ids
std::vector<std::string> Client::getAllRooms() {
    std::string join_data = "GET_ROOMS";
    sendMessage(join_data);
    responseHandler(_connection_socket);
    
    std::istringstream iss((std::string)_active_rooms);
    std::vector<std::string> data_set;
    std::string data;

    while (iss >> data) {
        data_set.push_back(data);
    }
    return data_set;
}

// Joining room with given id
void Client::joinRoom(std::string room_id) {
    std::string join_data = "VERIFYTOKEN " + _token + " JOIN_ROOM " + room_id;
    sendMessage(join_data);
}

// Exiting room with given id
void Client::exitRoom(std::string room_id) {
    std::string join_data = "VERIFYTOKEN " + _token + " EXIT_ROOM " + room_id;
    sendMessage(join_data);
}


// Response handler
void Client::responseHandler(SOCKET socket) {

    // wait for response from server
    char recvbuf[DEFAULT_BUFLEN];
    int iResult = recv(socket, recvbuf, DEFAULT_BUFLEN, 0);

    if (iResult > 0) {
        recvbuf[iResult] = '\0';
        std::cout << "Server response: " << recvbuf << "\n";

        // LOGIN HANDLING
        if (((std::string)recvbuf).substr(0, 13) == "LOGIN_SUCCESS") {

            _token = ((std::string)recvbuf).substr(14);
            std::cout << "Login successful.\n";

        }
        else if (((std::string)recvbuf).substr(0, 11) == "TOKEN_VALID") {
            std::cout << "Token confirmation successful.\n";

        }
        else if (((std::string)recvbuf).substr(0, 9) == "GET_ROOMS") {
            _active_rooms = ((std::string)recvbuf).substr(9);
            std::cout << "Rooms receiving successful.\n";
        }
        else {
        }
    }
    else {
        std::cerr << "recv failed: " << WSAGetLastError() << "\n";
        closesocket(socket);
        WSACleanup();
        return;
    }
}

// Start user input handling
void Client::handleInput(std::string input) {

    if (input[0] == '/') {
        std::istringstream iss(input);
        std::vector<std::string> tokens;
        std::string token;

        while (iss >> token) {
            tokens.push_back(token);
        }

        std::string command = tokens[0];
        if (command == "/Exit") {
            // TO DO: EXIT CURRENT CHAT ROOM
        }
    }
    else {
        
        std::string message = "VERIFYTOKEN " + _token + " MESSAGE " + input;
        sendMessage(message);
    }
}