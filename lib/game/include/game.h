#pragma once

#include "server.h"
#include "rules.h"
#include <string>
#include <vector>
#include <map>
#include <any>
#include <memory>

class Game {
public:
    Game(
        std::string name, uintptr_t ownerID, 
        int min_players, int max_players, bool audience,
        ElementSptr setup,
        ElementSptr constants, ElementSptr variables,
        ElementVector per_player, ElementVector per_audience, 
        std::vector<RuleUptr> rules
    );

    void start();
    bool isOngoing();

    std::string name();
    uintptr_t ownerID();
    uintptr_t id();

    void addPlayer(Connection playerID);
    void removePlayer(Connection playerID);
    bool hasPlayer(Connection playerID);
    std::vector<Connection> players();
    size_t numPlayers(); //number of players in vector above

private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string _name;
    uintptr_t _ownerID;
    bool _started;
    std::vector<Connection> _players;

    //Bounds of player given in json file
    struct PlayerCount {
        int min;
        int max;
    } _player_count;

    bool _audience;

    // Goal: map of { name_string -> { configurable_value , prompt_text } }
    ElementSptr _setup;

    ElementSptr _constants;
    ElementSptr _variables;
    ElementVector _per_player; // a list for each player
    ElementVector _per_audience; // a list for each audience member

    std::vector<RuleUptr> _rules;
};