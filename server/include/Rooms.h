#ifndef SERVER_ROOMS_H
#define SERVER_ROOMS_H

#include <iostream>
#include <vector>
#include <map>
#include <algorithm>

// Main class of chat rooms
// Featuring connecting multiple users
// and data storing
class Rooms {
private:
    struct Room {
        // vector of users : <username>
        std::vector< std::string > _chat_users;
        // config of room
        int _max_users; // max users allowed in room
    };

    // vector of rooms
    static std::map<std::string ,Room> _rooms;
    static int _room_counter; // TEMPORARY ROOM IDS

public:
    // constructors
    Rooms() = default;

    // Other methods

    // creates new chat room
    static void createRoom(int max_users);
    // deletes existing chat room with given id
    static void deleteRoom(const std::string& room_id);
    // appends new user to chat room
    static bool enterRoom(const std::string& username, const std::string& room_id);
    // removes user from chat room
    static void exitRoom(const std::string& username, const std::string& room_id);


    // Util methods
    // checks if user is in any room
    static std::string checkUser(const std::string& username);
    // checks if user is in room with given id
    static bool checkUserInRoom(const std::string& username, const std::string& room_id);
    // returns current chat rooms' ids
    static std::string getAllRoomId();
};


#endif //SERVER_ROOMS_H
