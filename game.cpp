#include "game.h"

Game::Game(std::string _name, int _min_players, int _max_players, bool _audience,
    std::map<std::string, std::pair<std::any, std::string>> _setup,
    std::map<std::string, List> _constants, std::map<std::string, List> _variables,
    std::map<std::string, List> _per_player, std::map<std::string, List> _per_audience,
    std::vector<Rule> _rules)
    : name(_name), player_count { _min_players, _max_players }, audience(_audience), setup(_setup), 
      constants(_constants), variables(_variables), per_player(_per_player), per_audience(_per_audience), rules(_rules) { }
void Game::start() { }