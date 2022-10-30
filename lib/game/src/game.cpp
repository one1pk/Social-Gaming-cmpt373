#include "game.h"

#include <iostream>
#include <algorithm>

Game::Game(
    std::string name, Connection owner, 
    unsigned min_players, unsigned max_players, bool has_audience,
    ElementSptr setup,
    ElementSptr constants, ElementSptr variables,
    ElementSptr per_player, ElementSptr per_audience, 
    std::shared_ptr<PlayerMap> players, std::shared_ptr<PlayerMap> audience,
    RuleVector rules
) : _name(name), _owner(owner), _started(false),
    _player_count{ min_players, max_players }, _has_audience(has_audience),
    _setup(setup),
    _constants{constants}, _variables(variables),
    _per_player(per_player), _per_audience(per_audience),
    _players(players), _audience(audience),
    _rules(rules)  {
    static uintptr_t shared_id_counter = 1; // gameIDs start at 1
    _id = shared_id_counter++;
}

// starts the game execution
void Game::start() {
    _started = true;
    std::cout << "Game Started\n";

    for (auto rule: _rules) {
        std::cout << "Rule Loop\n";
        rule->execute();
    }
}

// returns game status
bool Game::isOngoing() {
    return _started;
}

bool Game::addPlayer(Connection player_connection) {
    if (_players->size() < _player_count.max) {
        _players->insert({ player_connection,  _per_player->clone() });
        _players->at(player_connection)->setMapElement("id", std::make_shared<Element<int>>(player_connection.id));
        return true;
    } else {
        // game is full
        return false;
    }
}

bool Game::removePlayer(Connection player_connection) {
    if (_players->erase(player_connection)) {
        return true;
    } else {
        // player is not in game
        return false;
    }
}

bool Game::hasPlayer(Connection player_connection) {
    return _players->count(player_connection);
}

// return the list of player connections
std::vector<Connection> Game::players() {
    std::vector<Connection> connections;
    for(auto it = _players->begin(); it != _players->end(); it++) {
        connections.push_back(it->first);
    }
    return connections;
}

// returns the number of players in the game
size_t Game::numPlayers() {
    return _players->size();
}

std::string Game::name() {
    return _name;
}

Connection Game::owner() {
    return _owner;
}

uintptr_t Game::id() {
    return _id;
}