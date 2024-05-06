#include "Rooms.h"
std::map<std::string, Rooms::Room> Rooms::_rooms;
int Rooms::_room_counter = 0;

void Rooms::createRoom(int max_users) {
    std::vector<std::string> users;
    Room new_room = {users, max_users};
    _rooms[std::to_string(++_room_counter)] = new_room;
}

bool Rooms::enterRoom(const std::string& username, const std::string& room_id) {
    if(Rooms::checkUser(username).empty()) {
        if (_rooms[room_id]._max_users > _rooms[room_id]._chat_users.size()) {
            _rooms[room_id]._chat_users.push_back(username);
            std::cout << "User: " << username << " has joined room: " << room_id << "\n";
            return true;
        } else {
            std::cout << "User: " << username << " tried to join full room: " << room_id << "\n";
            return false;
        }
    }
}

void Rooms::exitRoom(const std::string& username, const std::string& room_id) {
    std::string room = Rooms::checkUser(username);
    if(!room.empty()) {
        // find user in room and remove its instance
        auto it = std::remove_if(std::begin(_rooms[room]._chat_users), std::end(_rooms[room]._chat_users),
                       [=](const std::string& user) { return (user == username); });
        _rooms[room]._chat_users.erase(it, _rooms[room]._chat_users.end());
    }
}

std::string Rooms::checkUser(const std::string& username) {
    for (auto& room : _rooms) {
        if (std::find(room.second._chat_users.begin(), room.second._chat_users.end(), username) != room.second._chat_users.end()) {
            return room.first;
        }
    }
    return "";
}

bool Rooms::checkUserInRoom(const std::string& username, const std::string& room_id) {
    auto room = _rooms.find(room_id);
    if (room != _rooms.end()) {
        return std::find(room->second._chat_users.begin(), room->second._chat_users.end(), username) != room->second._chat_users.end();
    }
    return false;
}

std::string Rooms::getAllRoomId() {
    std::string room_ids;
    for (const auto& room : _rooms) {
        room_ids.append(room.first + " ");
    }
    return room_ids;
}

void Rooms::deleteRoom(const std::string& room_id) {
    _rooms.erase(room_id);
}