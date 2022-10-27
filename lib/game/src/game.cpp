#include "game.h"

#include <iostream>
#include <algorithm>
using namespace ns;

Game::Game(){}

Game::Game(
    std::string name, uintptr_t ownerID, 
    int min_players, int max_players, bool audience,
    ElementSptr setup,
    ElementSptr constants, ElementSptr variables,
    ElementVector per_player, ElementVector per_audience, 
    std::vector<RuleUptr> rules
) : _name(name), _ownerID(ownerID), _started(false),
    _player_count{ min_players, max_players }, _audience(audience),
    _setup(setup),
    _constants{constants}, _variables(variables),
    _per_player(per_player), _per_audience(per_audience),
    _rules(std::move(rules))  {
    static uintptr_t shared_id_counter = 1; // gameIDs start at 1
    _id = shared_id_counter++;
}

// starts the game execution
void Game::start() {
    _started = true;
        std::cout << "Game Started\n";

    for (auto&& rule: _rules) {
        std::cout << "Rule Loop\n";

        rule->execute();
    }
}

// returns game status
bool Game::isOngoing() {
    return _started;
}

void Game::addPlayer(Connection playerID) {
    _players.push_back(playerID);
}

void Game::removePlayer(Connection playerID) {
    auto eraseBegin = std::remove(_players.begin(), _players.end(), playerID);
    _players.erase(eraseBegin, _players.end());
}

bool Game::hasPlayer(Connection playerID) {
    return std::find(_players.begin(), _players.end(), playerID) != _players.end();
}

// return the list of players
std::vector<Connection> Game::players() {
    return _players;
}

// returns the number of players in the game
size_t Game::numPlayers() {
    return _players.size();
}

// returns the name of the game
std::string Game::name() {
    return _name;
}

uintptr_t Game::ownerID() {
    return _ownerID;
}

uintptr_t Game::id() {
    return _id;
}