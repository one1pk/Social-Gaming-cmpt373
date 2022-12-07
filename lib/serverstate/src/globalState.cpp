#include "globalState.h"

void GlobalServerState::addNewUsers(std::vector<User>& users) {
    clients.insert(clients.end(), users.begin(), users.end());
    // clients_in_lobby.insert(clients_in_lobby.end(), users.begin(), users.end());
    users.clear();
}

void GlobalServerState::disconnectUser(User user) {
    removeClientFromList(clients, user);
    removeClientFromList(clients_in_lobby, user);
}

void GlobalServerState::addClientToGame(User user, uintptr_t invitationCode) {
    Game *game_instance = getGameInstancebyInvitation(invitationCode);

    game_instance->addPlayer(user, getName(user));

    clients_in_games[user] = game_instance->id();
    removeClientFromList(clients_in_lobby, user);
}

void GlobalServerState::addClientToLobby(User user) {
    clients_in_lobby.push_back(user);
}

void GlobalServerState::removeClientFromGame(User user) {
    uintptr_t gameID = clients_in_games[user];
    Game *game_instance = getGameInstancebyId(gameID);

    game_instance->removePlayer(user);
    clients_in_games.erase(user);
    clients_in_lobby.push_back(user);
}

///////////////////     GAME-RELATED FUNCTIONS     ///////////////////

void GlobalServerState::constructGame(std::vector<Game>& game_instances, std::string game_name, User owner) {    
    //Interpreter maps json info into game object and then returns the game 
    InterpretJson interpreter(game_name, owner);
    game_instances.emplace_back(interpreter.interpret());
}

uintptr_t GlobalServerState::createGame(int gameIndex, User user) {
    
    constructGame(game_instances, gameNameList[gameIndex], user);

    removeClientFromList(clients_in_lobby, user);
    clients_in_games[user] = game_instances.back().id();
    gameOwnerMap[user] = game_instances.back().id();

    return game_instances.back().id();
}

void GlobalServerState::startGame(User user) {
    Game *game_instance = getGameInstancebyUser(user);
    game_instance->run();
}

void GlobalServerState::endGame(User user) {
    Game *game_instance = getGameInstancebyUser(user);
    // FIX: Figure if needed here (related to createGame) (right now owner is added to clients in games)
    clients_in_games.erase(user);
    clients_in_lobby.push_back(user);

    // remove players
    for (auto &player : game_instance->players()) {
        clients_in_lobby.push_back(player);
        clients_in_games.erase(player);
    }

    gameOwnerMap.erase(user);
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
                    User user = input_request.user;
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

User
GlobalServerState::getGameOwner(User user){
    return getGameInstancebyUser(user)->owner();
}

int GlobalServerState::getPlayerCount(User user) {
    uintptr_t gameID = clients_in_games[user];
    return getGameInstancebyId(gameID)->numPlayers();
}

std::string GlobalServerState::getSetup(User user) {
    auto gameInstance = getGameInstancebyOwner(user);
    ElementSptr setup = gameInstance->setup();
    std::stringstream setupString;
    setupString << "Setup Variables:\n";
    //int index = 0;
    for(auto& [elementName, element] : setup->getMap()){
        setupString << elementName << "\n";
    }

    return setupString.str();
}

void GlobalServerState::setName(User user, std::string name) {
    userNames[user] = name;
}

std::string GlobalServerState::getName(User user) {
    if(userNames.find(user) != userNames.end()) {
        return userNames[user];
    }
    return "error";
}

bool GlobalServerState::isInLobby(User user){
    return std::find(clients_in_lobby.begin(), clients_in_lobby.end(), user) != clients_in_lobby.end();
}

bool GlobalServerState::isInGame(User user) {
    return clients_in_games.find(user) != clients_in_games.end();
}

bool GlobalServerState::isGameIndex(int index) {
    return gameNameList.find(index) != gameNameList.end();
}

bool GlobalServerState::isOwner(User user) {
    return gameOwnerMap.find(user) != gameOwnerMap.end();
}

bool GlobalServerState::gameHasEnoughPlayers(User user) {
    return getGameInstancebyUser(user)->hasEnoughPlayers();
}

bool GlobalServerState::isOngoingGame(User user) {
    Game *game_instance = getGameInstancebyUser(user);
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

void GlobalServerState::registerUserGameInput(User user, std::string input) {
    user_game_input[user].input = input;
    user_game_input[user].new_input = true;
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
GlobalServerState::buildMsgsForOtherPlayers(std::string messageText, User user) {
    std::deque<Message> messages;
    Game* game = getGameInstancebyUser(user);

    for (auto player : game->players()) {
        if (player == user) continue;
        messages.push_back({player, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMsgsForAllPlayers(std::string messageText, User user) {
    std::deque<Message> messages;
    Game* game = getGameInstancebyUser(user);

    for (auto player : game->players()) {
        messages.push_back({player, messageText});
    }
    return messages;
}

std::deque<Message>
GlobalServerState::buildMsgsForAllPlayersAndOwner(std::string messageText, User user) {
    std::deque<Message> messages = buildMsgsForAllPlayers(messageText, user);
    Game* game = getGameInstancebyUser(user);
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

void GlobalServerState::removeClientFromList(std::vector<User> &list, User user) {
    auto eraseBegin = std::remove(list.begin(), list.end(), user);
    list.erase(eraseBegin, list.end());
}

Game *
GlobalServerState::getGameInstancebyUser(User user) {
    uintptr_t gameID = clients_in_games[user];
    return getGameInstancebyId(gameID);
}

Game *
GlobalServerState::getGameInstancebyOwner(User user) {
    // WARNING: Assumes, owner exists;
    uintptr_t gameID = gameOwnerMap[user];

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

// compiles all the game names in ./gameconfigs into a list
// all games defined in ./gameconfigs can be "served" to users
void GlobalServerState::populateGameList() {
    gameNameList[0] = std::string("Rock_Paper_Scissors");
    gameNameList[1] = std::string("triviaGame");
}


void GlobalServerState::ConfigureSetupValue(User user, std::string key, int value){
        auto gameInstance = getGameInstancebyOwner(user);
        auto element = std::make_shared<Element<int>>(value);
        gameInstance->setup()->changeMapElement(key, element);
}