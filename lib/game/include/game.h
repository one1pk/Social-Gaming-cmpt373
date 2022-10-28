#pragma once

<<<<<<< HEAD
<<<<<<< HEAD
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
=======
#include "../../networking/include/server.h"
=======
#include "server.h"
>>>>>>> 3100b26 (Fix server include path)
#include  "rules.h"
#include <string>
#include <vector>
#include <map>
#include <any>
#include <nlohmann/json.hpp>
<<<<<<< HEAD
=======
#include <memory>
#include <iostream>
>>>>>>> d5888b9 (Fix naming inconsistencies)

using json = nlohmann::json;
using namespace std;
namespace ns{
class Game {
public:
    Game();
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
>>>>>>> 19b7927 (Interpret part of json and add test)

    void run();
    GameStatus status();

<<<<<<< HEAD
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
};
=======
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

    
    // map of { name_string -> { configurable_value , prompt_text } }
    std::map<std::string, std::pair<std::any, std::string>> setup;

<<<<<<< HEAD
    std::map<std::string, List> constants;
    std::map<std::string, List> variables;
    std::map<std::string, List> per_player;
    std::map<std::string, List> per_audience;
    std::vector<Rule> rules;
};


}
>>>>>>> 19b7927 (Interpret part of json and add test)
=======
    // Goal: map of { name_string -> { configurable_value , prompt_text } }
    ElementSptr _setup;

    ElementSptr _constants;
    ElementSptr _variables;
    ElementSptr _per_player; // a vector of lists for each player
    ElementSptr _per_audience; // a vector of lists for each audience member

    RuleVector _rules;
};
}
>>>>>>> cf6b5f2 (Add brace)
