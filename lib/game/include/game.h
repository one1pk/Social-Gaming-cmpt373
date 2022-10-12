#pragma once

#include "server.h"

#include <string>
#include <vector>


class Game {
public:
    Game(std::string name, uintptr_t ownerID);

    void start();

    void addPlayer(Connection playerID);
    bool hasPlayer(Connection playerID);
    std::vector<Connection> players();
    size_t numPlayers();


    std::string name();
    uintptr_t ownerID();
    uintptr_t id();

private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string _name;
    uintptr_t _ownerID;
    std::vector<Connection> _players;
};