#include "globalState.h"

void GlobalServerState::addConnection(Connection connection) {
    clients.push_back(connection);
    clients_in_lobby.push_back(connection);
}

void GlobalServerState::disconnectConnection(Connection connection) {
    removeClientFromList(clients, connection);
    removeClientFromList(clients_in_lobby, connection);
}

void GlobalServerState::addClientToGame(Connection connection, uintptr_t invitationCode) {
    Game *game_instance = getGameInstancebyInvitation(invitationCode);

    game_instance->addPlayer(connection);

    clients_in_games[connection] = game_instance->id();
    removeClientFromList(clients_in_lobby, connection);

}

void GlobalServerState::removeClientFromGame(Connection connection) {

    uintptr_t gameID = clients_in_games[connection];
    Game *game_instance = getGameInstancebyId(gameID);

    game_instance->removePlayer(connection);
    clients_in_games.erase(connection);
    clients_in_lobby.push_back(connection);
}

///////////////////     GAME- RELATED FUNCTIONS     ///////////////////

uintptr_t GlobalServerState::createGame(int gameIndex, Connection connection) {
    game_instances.emplace_back(GameNameStringsByIndex[gameIndex], connection.id);

    removeClientFromList(clients_in_lobby, connection);
    clients_in_games[connection] = game_instances.back().id();
    gameOwnerMap[connection] = game_instances.back().id();

    return game_instances.back().id();
}

void GlobalServerState::startGame(Connection connection) {
    Game *game_instance = getGameInstancebyOwner(connection);
    game_instance->start();
}

void GlobalServerState::endGame(Connection connection) {
    Game *game_instance = getGameInstancebyOwner(connection);
    // FIX: Figure if needed here (related to createGame)
    clients_in_games.erase(connection);
    clients_in_lobby.push_back(connection);

    // remove players
    for (auto &player : game_instance->players()) {
        clients_in_lobby.push_back(player);
        clients_in_games.erase(player);
    }

    gameOwnerMap.erase(connection);
    removeGameInstance(game_instance->id());
}

// All games methods
std::string
GlobalServerState::getGameNamesAsString() {
    std::stringstream gamesList;
    gamesList << "Available Games: \n";
    for (auto games : GameNameStringsByIndex) {
        gamesList << "[ " << games.first << " ] " << games.second << "\n";
    }

    return gamesList.str();
}

int 
GlobalServerState::getPlayerCount(Connection connection){
    uintptr_t gameID = clients_in_games[connection];
    return getGameInstancebyId(gameID)->numPlayers();
}

bool GlobalServerState::isCurrentlyInGame(Connection connection) {
    return clients_in_games.find(connection) != clients_in_games.end();
}

bool GlobalServerState::isGameIndex(int index) {
    return GameList.find(index) != GameList.end();
}

bool GlobalServerState::isOwner(Connection connection) {
    return gameOwnerMap.find(connection) != gameOwnerMap.end();
}

bool GlobalServerState::isOngoingGame(Connection connection) {

    Game *game_instance = getGameInstancebyOwner(connection);
    return game_instance->isOngoing();
}

bool GlobalServerState::isValidGameInvitation(uintptr_t invitationCode) {
    Game *game_instance = getGameInstancebyInvitation(invitationCode);
    return game_instance == nullptr ? false : true;
}

//////////////////////////////      BROADCASTING MESSAGE BUILDERS   //////////////////////

std::deque<Message>
GlobalServerState::buildMessagesForLobby(std::string messageText) {
    std::deque<Message> messages;
    for (auto client : clients_in_lobby) {
        messages.push_back({client, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMessagesForGame(std::string messageText, Connection connection) {
    std::deque<Message> messages;

    if (clients_in_games.find(connection) != clients_in_games.end()) {
        uintptr_t gameID = clients_in_games[connection];
        Game *game_instance = getGameInstancebyId(gameID);

        for (auto player : game_instance->players()) {
            if(!(player==connection)){
                messages.push_back({player, messageText});
            }
        }

        // TODO: Send to owner as well
        // messages.push_back({game_instance->owner(), messageText});
    }

    return messages;
}

//////////////////////////////      PRIVATE METHODS     /////////////////////////////////

void GlobalServerState::removeGameInstance(uintptr_t gameID) {
    auto eraseEnd = std::remove_if(game_instances.begin(), game_instances.end(), [gameID](auto &game) { return game.id() == gameID; });

    game_instances.erase(eraseEnd, game_instances.end());
}

void GlobalServerState::removeClientFromList(std::vector<Connection> &list, Connection connection) {
    auto eraseBegin = std::remove(list.begin(), list.end(), connection);
    list.erase(eraseBegin, list.end());
}

Game *
GlobalServerState::getGameInstancebyOwner(Connection connection) {
    // WARNING: Assumes, owner exists;
    uintptr_t gameID = gameOwnerMap[connection];

    auto findGamePredicate = [gameID](auto &game) { return game.id() == gameID; };
    auto game_iterator = std::find_if(game_instances.begin(), game_instances.end(), findGamePredicate);
    return &(*game_iterator);
}

Game *
GlobalServerState::getGameInstancebyInvitation(uintptr_t invitationCode) {

    auto findGamePredicate = [invitationCode](auto &game) { return game.id() == invitationCode; };
    auto game_iterator = std::find_if(game_instances.begin(), game_instances.end(), findGamePredicate);

    return game_iterator == game_instances.end() ? nullptr : &(*game_iterator);
}

Game *
GlobalServerState::getGameInstancebyId(uintptr_t gameID) {

    auto findGamePredicate = [gameID](auto &game) { return game.id() == gameID; };
    auto game_iterator = std::find_if(game_instances.begin(), game_instances.end(), findGamePredicate);

    return game_iterator == game_instances.end() ? nullptr : &(*game_iterator);
}