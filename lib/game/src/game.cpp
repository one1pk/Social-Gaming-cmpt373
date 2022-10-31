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
    RuleVector rules,
    std::shared_ptr<std::deque<Message>> player_msgs,
    std::shared_ptr<std::deque<std::string>> global_msgs
) : _name(name), _owner(owner), _started(false),
    _player_count{ min_players, max_players }, _has_audience(has_audience),
    _setup(setup),
    _constants{constants}, _variables(variables),
    _per_player(per_player), _per_audience(per_audience),
    _players(players), _audience(audience),
    _rules(rules),
    _player_msgs(player_msgs), _global_msgs(global_msgs)  {
    static uintptr_t shared_id_counter = 1; // gameIDs start at 1
    _id = shared_id_counter++;
}

// starts the game execution
void Game::start() {
    _started = true;
    std::cout << "Game Started\n"
              << "Starting Rule Loop\n";

    for (auto rule: _rules) {
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
        _players->at(player_connection)->setMapElement(
            "connection", std::make_shared<Element<Connection>>(player_connection)
        );
        _players->at(player_connection)->setMapElement(
            "name", std::make_shared<Element<std::string>>(std::to_string(player_connection.id))
        );
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

std::deque<Message> Game::playerMsgs() {
    std::deque<Message> tmp = *_player_msgs;
    _player_msgs->clear();
    return tmp;
}

std::deque<std::string> Game::globalMsgs() {
    std::deque<std::string> tmp = *_global_msgs;
    _global_msgs->clear();
    return tmp;
}