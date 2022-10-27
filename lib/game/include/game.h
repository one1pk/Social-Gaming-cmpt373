#pragma once

#include "../../networking/include/server.h"
#include  "rules.h"
#include <string>
#include <vector>
#include <map>
#include <any>
#include <nlohmann/json.hpp>

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


    std::string getName();
    void printInfo(){
        cout << name << " " << audience << " " << player_count.min << " " << player_count.max << endl;
    }

    uintptr_t ownerID();
    uintptr_t id();
    
    NLOHMANN_DEFINE_TYPE_INTRUSIVE(Game, name, audience, player_count)
    
private:
    uintptr_t _id; // unique id can act as an invitation code
    std::string name;
    bool audience;
    uintptr_t _ownerID;
    bool _started;
    std::vector<Connection> _players;

    //Bounds of player given in json file
    struct PlayerCount {
        int min;
        int max;
        NLOHMANN_DEFINE_TYPE_INTRUSIVE(PlayerCount, min, max)
    } player_count;

    
    // map of { name_string -> { configurable_value , prompt_text } }
    std::map<std::string, std::pair<std::any, std::string>> setup;

    std::map<std::string, List> constants;
    std::map<std::string, List> variables;
    std::map<std::string, List> per_player;
    std::map<std::string, List> per_audience;
    std::vector<Rule> rules;
};


}