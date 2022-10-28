#pragma once

#include "server.h"
#include  "rules.h"
#include <string>
#include <vector>
#include <map>
#include <any>
#include <nlohmann/json.hpp>
#include <memory>
#include <iostream>

using json = nlohmann::json;
using namespace std;

namespace ns{

class Game {
public:
    Game();
    Game(
        std::string name, uintptr_t ownerID, 
        int min_players, int max_players, bool audience,
        ElementSptr setup,
        ElementSptr constants, ElementSptr variables,
        ElementSptr per_player, ElementSptr per_audience, 
        RuleVector rules
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
    //number of players in vector above
    size_t numPlayers();


    std::string getName();
    void printInfo(){
        cout << _name << " " << _audience << " " << _player_count.min << " " << _player_count.max << endl;
    }

    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Game, _name, _audience, _player_count)
    
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
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerCount, min, max)
    } _player_count;

    bool _audience;

    // Goal: map of { name_string -> { configurable_value , prompt_text } }
    ElementSptr _setup;

    ElementSptr _constants;
    ElementSptr _variables;
    ElementSptr _per_player; // a vector of lists for each player
    ElementSptr _per_audience; // a vector of lists for each audience member

    RuleVector _rules;
};
}
