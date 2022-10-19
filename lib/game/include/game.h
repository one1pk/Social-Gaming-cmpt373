#pragma once

#include "server.h"
#include  "rules.h"
#include <string>
#include <vector>
#include <map>
#include <any>


class Game {
public:
    Game(std::string name, uintptr_t ownerID);
    Game(std::string _name, int min_players, int max_players, bool _audience,
        std::map<std::string, std::pair<std::any, std::string>> _setup,
        std::map<std::string, List> _constants, std::map<std::string, List> _variables,
        std::map<std::string, List> _per_player, std::map<std::string, List> _per_audience,
        std::vector<Rule> _rules);

    void start();
    bool isOngoing();

    void addPlayer(Connection playerID);
    void removePlayer(Connection playerID);
    bool hasPlayer(Connection playerID);
    std::vector<Connection> players();
    //number of players in vector above
    size_t numPlayers();


    std::string name();
    uintptr_t ownerID();
    uintptr_t id();

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
    } player_count;

    bool audience;
    // map of { name_string -> { configurable_value , prompt_text } }
    std::map<std::string, std::pair<std::any, std::string>> setup;

    std::map<std::string, List> constants;
    std::map<std::string, List> variables;
    std::map<std::string, List> per_player;
    std::map<std::string, List> per_audience;
    std::vector<Rule> rules;
};