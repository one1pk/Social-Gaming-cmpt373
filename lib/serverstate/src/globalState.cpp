#include "globalState.h"
#include <glog/logging.h>

void GlobalServerState::addNewConnections(std::vector<Connection>& connections) {
    clients.insert(clients.end(), connections.begin(), connections.end());
    clients_in_lobby.insert(clients_in_lobby.end(), connections.begin(), connections.end());
    connections.clear();
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

///////////////////     GAME-RELATED FUNCTIONS     ///////////////////
void TEMP_ManualRpsGameConstruction(std::vector<Game>& game_instances, std::string game_name, Connection owner);
uintptr_t GlobalServerState::createGame(int gameIndex, Connection connection) {
    /// TODO: use interpreter to retrieve the appropriate game object corresponding to the game name
    
    TEMP_ManualRpsGameConstruction(game_instances, gameNameList[gameIndex], connection);

    removeClientFromList(clients_in_lobby, connection);
    clients_in_games[connection] = game_instances.back().id();
    gameOwnerMap[connection] = game_instances.back().id();

    return game_instances.back().id();
}

void GlobalServerState::startGame(Connection connection) {
    Game *game_instance = getGameInstancebyUser(connection);
    game_instance->run();
}

void GlobalServerState::endGame(Connection connection) {
    Game *game_instance = getGameInstancebyUser(connection);
    // FIX: Figure if needed here (related to createGame) (right now owner is added to clients in games)
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

bool is_number(std::string_view s) {
    return !s.empty() && std::find_if(s.begin(), s.end(), 
        [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}

std::deque<Message> GlobalServerState::processGames() {
    std::deque<Message> outgoing;

    auto game = game_instances.begin();
    while (game != game_instances.end()) {
        // LOG(INFO) << "Game Check";
        switch (game->status()) {
            case GameStatus::AwaitingOutput: {
                LOG(INFO) << "game " << game->id() << ": AwaitingOutput";
                processGameMsgs(*game, outgoing);
                game->outputSent(); // changes the status to AwaitingInput
                game++;
            }
            break;
            case GameStatus::AwaitingInput: {
                LOG(INFO) << "game " << game->id() << ": AwaitingInput";
                std::deque<InputRequest> input_requests = game->inputRequests();
                for (auto input_request: input_requests) {
                    Connection user = input_request.user;
                    if (user_game_input[user].new_input) {
                        std::string input = user_game_input[user].input;

                        // check if valid input
                        if (input_request.type != InputType::Text &&  
                                (!is_number(input) || (unsigned)std::stoi(input) >= input_request.num_choices)) {
                            std::stringstream msg;
                            msg << "Invalid index, please enter a number between 0 and " << input_request.num_choices-1 << "\n";
                            outgoing.push_back({ user, msg.str() });
                            user_game_input[user].new_input = false;
                            continue;
                        }

                        game->registerPlayerInput(user, input);
                        outgoing.push_back({
                            user,
                            "Input Received, you entered: " + input + "\n"
                            "Waiting for other players...\n\n"
                        });
                    } else if (input_request.has_timeout) {
                        user_game_input[user].time_remaining -= update_interval;
                        if (user_game_input[user].time_remaining <= 0) {
                            game->inputRequestTimedout(user);
                            outgoing.push_back({
                                user,
                                "Input window timed out!\n"
                                "Selecting index 0\n\n"
                            });
                        }
                    }
                }

                if (game->inputRequests().size() == 0) {
                    game->run();
                }
                game++;
            }
            break;
            case GameStatus::Finished: {
                LOG(INFO) << "game " << game->id() << ": GameFinished";
                processGameMsgs(*game, outgoing);
                outgoing.push_back({game->owner(), "\nThe game has finished!\nReturning to the lobby\n\n"});
                std::deque<Message> finalMsgs = buildMsgsForOtherPlayers("\nGood game!\nYou are now back in the lobby\n\n", game->owner());
                outgoing.insert(outgoing.end(), finalMsgs.begin(), finalMsgs.end());
                endGame(game->owner());
            }
            break;
            default:
                game++;
            break;
        }
    }
    return outgoing;
}

// All games methods
std::string
GlobalServerState::getGameNamesAsString() {
    std::stringstream gamesList;
    gamesList << "Available Games: \n";
    for (auto games : gameNameList) {
        gamesList << "[ " << games.first << " ] " << games.second << "\n";
    }
    gamesList << "\n";

    return gamesList.str();
}

Connection
GlobalServerState::getGameOwner(Connection connection){
    return getGameInstancebyUser(connection)->owner();
}

int GlobalServerState::getPlayerCount(Connection connection) {
    uintptr_t gameID = clients_in_games[connection];
    return getGameInstancebyId(gameID)->numPlayers();
}

bool GlobalServerState::isInGame(Connection connection) {
    return clients_in_games.find(connection) != clients_in_games.end();
}

bool GlobalServerState::isGameIndex(int index) {
    return gameNameList.find(index) != gameNameList.end();
}

bool GlobalServerState::isOwner(Connection connection) {
    return gameOwnerMap.find(connection) != gameOwnerMap.end();
}

bool GlobalServerState::gameHasEnoughPlayers(Connection connection) {
    return getGameInstancebyUser(connection)->hasEnoughPlayers();
}

bool GlobalServerState::isOngoingGame(Connection connection) {
    Game *game_instance = getGameInstancebyUser(connection);
    return game_instance->status() != GameStatus::Finished
        && game_instance->status() != GameStatus::Created;
}

bool GlobalServerState::isOngoingGame(uintptr_t invitation_code) {
    Game *game_instance = getGameInstancebyInvitation(invitation_code);
    return game_instance->status() != GameStatus::Finished
        && game_instance->status() != GameStatus::Created;
}

bool GlobalServerState::isValidGameInvitation(uintptr_t invitation_code) {
    Game *game_instance = getGameInstancebyInvitation(invitation_code);
    return game_instance != nullptr;
}

void GlobalServerState::registerUserGameInput(Connection connection, std::string input) {
    user_game_input[connection].input = input;
    user_game_input[connection].new_input = true;
}

//////////////////////////////      BROADCASTING MESSAGE BUILDERS   //////////////////////

std::deque<Message>
GlobalServerState::buildMessagesForServerLobby(std::string messageText) {
    std::deque<Message> messages;
    for (auto client : clients_in_lobby) {
        messages.push_back({client, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMsgsForOtherPlayers(std::string messageText, Connection connection) {
    std::deque<Message> messages;
    Game* game = getGameInstancebyUser(connection);

    for (auto player : game->players()) {
        if (player == connection) continue;
        messages.push_back({player, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMsgsForAllPlayers(std::string messageText, Connection connection) {
    std::deque<Message> messages;
    Game* game = getGameInstancebyUser(connection);

    for (auto player : game->players()) {
        messages.push_back({player, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMsgsForAllPlayersAndOwner(std::string messageText, Connection connection) {
    std::deque<Message> messages = buildMsgsForAllPlayers(messageText, connection);
    Game* game = getGameInstancebyUser(connection);
    messages.push_back({game->owner(), messageText});
    return messages;
}

//////////////////////////////      PRIVATE METHODS     /////////////////////////////////

void GlobalServerState::processGameMsgs(Game& game, std::deque<Message>& outgoing) {
    // add the global msg strings to the outgoing message list (main screen)
    std::deque<std::string> global_msg_strings = game.globalMsgs();
    std::transform(global_msg_strings.begin(), global_msg_strings.end(), std::back_inserter(outgoing),
        [&game](auto global_msg_string) {
            return Message{ game.owner(), global_msg_string };
        }
    );

    // add the player input request prompts to the outgoing message list (player screens)
    std::deque<InputRequest> input_requests = game.inputRequests();
    std::transform(input_requests.begin(), input_requests.end(), std::back_inserter(outgoing),
        [](auto input_request) {
            return Message{ input_request.user, input_request.prompt };
        }
    );

    // flag the users requiring input (if the game is not finished)
    for (auto input_request: input_requests) {
        user_game_input[input_request.user].new_input = false;
        user_game_input[input_request.user].time_remaining = input_request.timeout_ms;
    }
}

void GlobalServerState::removeGameInstance(uintptr_t gameID) {
    auto eraseEnd = std::remove_if(game_instances.begin(), game_instances.end(), [gameID](auto &game) { return game.id() == gameID; });

    game_instances.erase(eraseEnd, game_instances.end());
}

void GlobalServerState::removeClientFromList(std::vector<Connection> &list, Connection connection) {
    auto eraseBegin = std::remove(list.begin(), list.end(), connection);
    list.erase(eraseBegin, list.end());
}

Game *
GlobalServerState::getGameInstancebyUser(Connection connection) {
    uintptr_t gameID = clients_in_games[connection];
    return getGameInstancebyId(gameID);
}

// Game *
// GlobalServerState::getGameInstancebyOwner(Connection connection) {
//     // WARNING: Assumes, owner exists;
//     uintptr_t gameID = gameOwnerMap[connection];

//     auto findGamePredicate = [gameID](auto &game) { return game.id() == gameID; };
//     auto game_iterator = std::find_if(game_instances.begin(), game_instances.end(), findGamePredicate);
//     return &(*game_iterator);
// }

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

// compiles all the game names in ./gameconfigs into a list
// all games defined in ./gameconfigs can be "served" to users
void GlobalServerState::populateGameList() {
    gameNameList[0] = std::string("Rock Paper Scissors");
    gameNameList[1] = std::string("TestGame");
}




////////////////////////////////////////////////////////////////////////////////////////////
////////////// TEMPORARY: Manual Rock Paper Scissor Game Construction //////////////////////
#include "InterpretJson.h"


void TEMP_ManualRpsGameConstruction(std::vector<Game>& game_instances, std::string game_name, Connection owner) {    
    //Interpreter maps json info to game 
    InterpretJson j("rock_paper_scissors", owner);
    game_instances.emplace_back(j.interpret());
}