#include "Settings.h"

// Constructors
Server::Server(const char *port) {
    // Initialize Winsock
    _response_result = WSAStartup(MAKEWORD(2,2), &_wsa_data);

    if (_response_result) {
        std::cout << "WSAStartup failed: \n" << _response_result;
        return;
    }

    ZeroMemory(&hints, sizeof (hints));
    hints.ai_family = AF_INET; // ipv4 specifier / AF_INET6 for ipv6
    hints.ai_socktype = SOCK_STREAM; // type of socket
    hints.ai_protocol = IPPROTO_TCP; // TCP connection
    hints.ai_flags = AI_PASSIVE; // Flags

    // Resolve the local address and port to be used by the server
    _response_result = getaddrinfo(nullptr, port, &hints, &_addrinfo_result);
    if (_response_result) {
        std::cout << "getaddrinfo failed: \n" << _response_result << "\n";
        serverCleanup();
        return;
    }

    _bufflen = DEFAULT_BUFLEN;
    _port = port;
};

Server::Server() : Server(DEFAULT_PORT) {}
Server::~Server() {
    serverCleanup();
}

// Util methods
// run server
void Server::runServer() {

    openListener();
    std::cout << "Server started on port: " << _port << "\n";

    // TEMPORARY: CREATE TESTING CHAT ROOMS
    Rooms::createRoom(1);
    Rooms::createRoom(20);

    // infinite loop
    while(_is_running) {
        SOCKET client_socket = SOCKET_ERROR;
        if(client_socket = acceptConnection(); client_socket != SOCKET_ERROR) {
            std::thread clientThread(&Server::handleConnection, this, client_socket);
            clientThread.detach();
        }
    }

    serverCleanup();
}

// stops server loop
void Server::stopServer() {
    std::cout << "Shutting down the server...\n";
    _is_running = 0;
}

// clear all connections
void Server::serverCleanup() {
    stopServer();

    if(_addrinfo_result != nullptr){
        freeaddrinfo(_addrinfo_result);
    }

    WSACleanup();
}

// initialize listener socket
void Server::openListener() {
    // Create a SOCKET for the server to listen for client connections
    // ensure it is a valid socket
    check(_listener_socket = socket(_addrinfo_result->ai_family, _addrinfo_result->ai_socktype,
                                    _addrinfo_result->ai_protocol));

    // Binding listening TCP socket with network addr and port
    check(_response_result = bind(_listener_socket, _addrinfo_result->ai_addr, (int) _addrinfo_result->ai_addrlen));

    // Listening on sockets
    // (created socket, maximum of connections at queue to accept)
    check(listen(_listener_socket, SOMAXCONN));
}

// accepts new connection
SOCKET Server::acceptConnection() {
    sockaddr_in client;
    int client_size = sizeof(client);
    SOCKET client_socket;
    check(client_socket = accept(_listener_socket, (sockaddr *) &client, &client_size));
    std::cout << "Client connected.\n";
    return client_socket;
}


// Handling connections and requests ==================================
// ====================================================================
void Server::handleConnection(SOCKET client_socket) {
    char recvbuf[_bufflen];
    int bytes_received;

    while (true) {
        ZeroMemory(recvbuf, _bufflen);
        bytes_received = recv(client_socket, recvbuf, _bufflen, 0);

        if (bytes_received == 0) {
            std::cout << "Client disconnected.\n";
        } else if (bytes_received < 0) {
            std::cout << "Error in receiving bytes.\n";
            break;
        }
        std::string session_response = processRequest(recvbuf, client_socket);
        std::cout << "Received ( " << bytes_received << "bytes ) with message: \"" << recvbuf << "\"\n";
        send(client_socket, session_response.c_str(), session_response.length(), 0);

        if (session_response == "TOKEN_VALID") {
            // Processing rest of data e.g. choosing chat room / sending messages
            //((std::string)recvbuf).substr(11);
        }

    }

    closesocket(client_socket);
}

// handle message sending
void Server::handleChatMessage(const std::string& token, const std::string& message) {
    for (const auto& [key, value] : SessionManager::sessions) {
        if(token != key && value.chat_id == SessionManager::sessions[token].chat_id) {
            std::string data = SessionManager::sessions[token].username + " " + message;
            send(*value.client_socket, data.c_str(), data.length(), 0);
        }
    }
}

// Utility functions =================================================
// ===================================================================

// handle common winsocket2 errors ---------------
int Server::check(int i) {
    if(i != 0) {
        std::cout << "Error occurred.\nError num: " << i << "\n";
        //serverCleanup();
        return 0;
    } return 1;
}

int Server::check(SOCKET socket) {
    if(socket == SOCKET_ERROR) {
        std::cout << "Error occurred.\nError num: " << WSAGetLastError() << "\n";
        //serverCleanup();
        return 0;
    } return 1;
}


// Session management ================================================
// ===================================================================
// session processing
std::string Server::processRequest(const std::string &request, SOCKET client_socket) {
    std::istringstream iss(request);
    std::vector<std::string> requests;
    std::string current_request;

    while (iss >> current_request) {
        requests.push_back(current_request);
    }


    // Log In
    if (requests[0] == "LOGIN") {
        // LOGIN <token> <requestID> <data>
        std::string username = requests[1];
        std::string token = SessionManager::startSession(username, client_socket);
        return "LOGIN_SUCCESS " + token;

    } else if (requests[0] == "VERIFYTOKEN") {
        std::string token = requests[1];

        if (SessionManager::verifySessionToken(token)) {
            if (requests[2] == "MESSAGE") {
                std::string message;
                std::for_each(requests.begin() + 3, requests.end(), [&](const std::string& req) {
                    message.append(req + " ");
                });
                handleChatMessage(token, message);
            } else if (requests[2] == "JOIN_ROOM") {
                if(Rooms::enterRoom(SessionManager::sessions[token].username, requests[3])) {
                    SessionManager::sessions[token].chat_id = requests[3];
                }
            } else if (requests[2] == "EXIT_ROOM") {
                Rooms::exitRoom(SessionManager::sessions[token].username, requests[3]);
                SessionManager::sessions[token].chat_id = "";
            }
            return "TOKEN_VALID";
        } else {
            return "TOKEN_INVALID";
        }
    } else if (requests[0] == "GET_ROOMS") {
        return "GET_ROOMS" + Rooms::getAllRoomId();
    } else {
        return "INVALID_REQUEST";
    }
}