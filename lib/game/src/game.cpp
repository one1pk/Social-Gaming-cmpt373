#include "game.h"

#include <iostream>
#include <algorithm>
using namespace ns;

Game::Game(){}
Game::Game(std::string _name, uintptr_t ownerID) 
    : name(_name), _ownerID(ownerID), _started(false) {
    static uintptr_t shared_id_counter = 1; // gameIDs start at 1
    _id = shared_id_counter++;
}

// starts the game execution
void Game::start() {
    _started = true;
}

// starts the game execution
bool Game::isOngoing() {
    return _started;
}

// adds a player to the game
void Game::addPlayer(Connection playerID) {
    _players.push_back(playerID);
}

void Game::removePlayer(Connection playerID) {
    auto eraseBegin = std::remove(_players.begin(), _players.end(), playerID);
    _players.erase(eraseBegin, _players.end());
}

// checks if a player is in the game
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
std::string Game::getName() {
    return name;
}

// returns the ownerID of the game
uintptr_t Game::ownerID() {
    return _ownerID;
}

// returns the gameID
uintptr_t Game::id() {
    return _id;
}