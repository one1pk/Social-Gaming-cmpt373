#pragma once

#include "game.h"
#include "server.h"

#include <sstream>
#include <unordered_map>
#include <vector>
#include <algorithm>

enum Games {
    ROCK_PAPER_SCISSORS,
};

class GlobalServerState {
public:
    GlobalServerState(){};

    // Client methods
    void addConnection(Connection);
    void disconnectConnection(Connection);

    void addClientToGame(Connection, uintptr_t invitationCode); // automatically removes from lobby
    void removeClientFromGame(Connection);                      // automatically adds client to lobby

    uintptr_t createGame(int gameIndex, Connection connection); // creates and removes owner from lobby
    void startGame(Connection connection);
    void endGame(Connection connection);

    // All games methods
    std::string getGameNamesAsString(); // returns all available games as string
    int getPlayerCount(Connection connection);
    bool isCurrentlyInGame(Connection connection);
    bool isGameIndex(int index);
    bool isOwner(Connection connection);
    bool isOngoingGame(Connection connection);
    bool isValidGameInvitation(uintptr_t invitationCode);

    // Broadcasting methods
    std::deque<Message> buildMessagesForLobby(std::string);
    std::deque<Message> buildMessagesForGame(std::string, Connection);


private:
    std::unordered_map<Connection, uintptr_t, ConnectionHash> clients_in_games;
    std::unordered_map<Connection, uintptr_t, ConnectionHash> gameOwnerMap;
    std::vector<Connection> clients;
    std::vector<Connection> clients_in_lobby;
    std::vector<Game> game_instances;

    // TODO: find better way to do this
    std::unordered_map<int, Games> GameList{
        {0, Games::ROCK_PAPER_SCISSORS},
    };

    std::unordered_map<int, std::string> GameNameStringsByIndex{
        {0, "Rock Paper Scissors"},
    };

    // void addClientToLobby(Connection c);
    // void removeClientFromLobby(Connection c);
    void removeGameInstance(uintptr_t gameID);
    void removeClientFromList(std::vector<Connection> &list, Connection connection);

    Game *getGameInstancebyOwner(Connection connection);
    Game *getGameInstancebyInvitation(uintptr_t invitationCode); // TODO: FIX Invitation code in game
    Game *getGameInstancebyId(uintptr_t gameID);
};