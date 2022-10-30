#pragma once

#include "server.h"
#include "rules.h"
#include "list.h"

#include <string>
#include <vector>
#include <map>
#include <any>
#include <memory>

class Game {
public:
    Game(
        std::string name, Connection owner, 
        unsigned min_players, unsigned max_players, bool has_audience,
        ElementSptr setup,
        ElementSptr constants, ElementSptr variables,
        ElementSptr per_player, ElementSptr per_audience, 
        std::shared_ptr<PlayerMap> players, std::shared_ptr<PlayerMap> audience,
        RuleVector rules
    );

    void start();
    bool isOngoing();

    std::string name();
    Connection owner();
    uintptr_t id();

    bool addPlayer(Connection playerID);
    bool removePlayer(Connection playerID);
    bool hasPlayer(Connection playerID);
    std::vector<Connection> players();
    size_t numPlayers(); //number of players in vector above

private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string _name;
    Connection _owner;
    bool _started;

    //Bounds of player given in json file
    struct PlayerCount {
        unsigned min;
        unsigned max;
    } _player_count;

    bool _has_audience;

    // Goal: map of { name_string -> { configurable_value , prompt_text } }
    ElementSptr _setup;

    ElementSptr _constants;
    ElementSptr _variables;

    ElementSptr _per_player; // a map template for players
    ElementSptr _per_audience; // a map template for audience members
    std::shared_ptr<PlayerMap> _players;  // maps each player to their game map
    std::shared_ptr<PlayerMap> _audience; // maps each audience to their game map

    RuleVector _rules;
};