#pragma once

#include "rules.h"
#include "list.h"
#include <vector>

class Game {
public:
    Game(std::string _name, int min_players, int max_players, bool _audience,
        std::map<std::string, std::pair<std::any, std::string>> _setup,
        std::map<std::string, List> _constants, std::map<std::string, List> _variables,
        std::map<std::string, List> _per_player, std::map<std::string, List> _per_audience,
        std::vector<Rule> _rules);

    void start();

private:
    std::string name;
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


