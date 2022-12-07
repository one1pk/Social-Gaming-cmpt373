#pragma once

#include "server.h"
#include "rules.h"
#include "list.h"

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <stack>

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
        std::string name, User owner
    );

    void run();
    GameStatus status();

    std::string name();
    User owner();
    uintptr_t id();

    ElementSptr setup();
    ElementSptr constants();
    ElementSptr variables();
    ElementSptr per_player();
    ElementSptr per_audience();
    RuleVector& rules();

    bool addPlayer(User playerID, std::string userName);
    bool removePlayer(User playerID);
    bool hasPlayer(User playerID);
    std::vector<User> players();
    unsigned numPlayers();
    bool hasEnoughPlayers();

    std::deque<std::string> globalMsgs();
    std::deque<InputRequest> inputRequests();

    void outputSent();
    void registerPlayerInput(User player, std::string input);
    void inputRequestTimedout(User player);


    ///TEMP: for interpreter
    void setOwner(User owner){ _owner = owner; }
    bool audience(){ return _has_audience; }
    void setName(std::string name) { _name = name; }
    void setRules(RuleVector rules) { _rules = rules; }
    void setID();

// private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string _name;
    User _owner;
    GameStatus _status;

    //Bounds of player given in json file
    struct PlayerCount {
        unsigned min;
        unsigned max;
    } _player_count;

    bool _has_audience;

    ElementMap _game_state;

    ElementSptr _per_player; // a map template for players
    ElementSptr _per_audience; // a map template for audience members
    RuleVector _rules;

    std::shared_ptr<PlayerMap> _players = std::make_shared<PlayerMap>(PlayerMap{}); // maps each player to their game map
    std::shared_ptr<PlayerMap> _audience = std::make_shared<PlayerMap>(PlayerMap{}); // maps each audience to their game map
    std::shared_ptr<std::deque<std::string>> _global_msgs = std::make_shared<std::deque<std::string>>();
    std::shared_ptr<std::deque<InputRequest>> _input_requests = std::make_shared<std::deque<InputRequest>>();
    std::shared_ptr<std::map<User, InputResponse>> _player_input = std::make_shared<std::map<User, InputResponse>>();
};
