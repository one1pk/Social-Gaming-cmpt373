#pragma once

#include "server.h"
#include "rules.h"
#include "list.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stack>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

enum GameStatus {
    Created,
    Running,
    Finished,
    AwaitingInput,
    AwaitingOutput
};

class Game {
public:
    Game();
    Game(
        std::string name, Connection owner, 
        unsigned min_players, unsigned max_players, bool has_audience,
        ElementSptr setup,
        ElementSptr constants, ElementSptr variables,
        ElementSptr per_player, ElementSptr per_audience, 
        std::shared_ptr<PlayerMap> players, std::shared_ptr<PlayerMap> audience,
        RuleVector rules,
        std::shared_ptr<std::deque<Message>> _player_msgs,
        std::shared_ptr<std::deque<std::string>> _global_msgs,
        std::shared_ptr<std::map<Connection, std::string>> _player_input
    );

    void run();
    GameStatus status();

    std::string name();
    Connection owner();
    uintptr_t id();

    bool addPlayer(Connection playerID);
    bool removePlayer(Connection playerID);
    bool hasPlayer(Connection playerID);
    std::vector<Connection> players();
    size_t numPlayers();

    std::deque<Message> playerMsgs();
    std::deque<std::string> globalMsgs();

    void outputSent();
    void registerPlayerInput(Connection player, std::string input);

    ElementSptr setup();
    ElementSptr constants();
    ElementSptr variables();
    ElementSptr per_player();
    ElementSptr per_audience();
    bool audience(){
        return _has_audience;
    }


private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string _name;
    Connection _owner;
    GameStatus _status;

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

    std::shared_ptr<std::deque<Message>> _player_msgs;
    std::shared_ptr<std::deque<std::string>> _global_msgs;
    std::shared_ptr<std::map<Connection, std::string>> _player_input;

    //allows from_json and to_json to access private fields
    friend void from_json(const json &j, Game &g);
    friend void to_json(json &j, const Game &g);

};
