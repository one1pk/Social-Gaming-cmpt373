#include "globalState.h"

void GlobalServerState::addConnection(Connection connection) {
    clients.push_back(connection);
    clients_in_lobby.push_back(connection);
}

void GlobalServerState::disconnectConnection(Connection connection) {
    removeClientFromList(clients, connection);
    removeClientFromList(clients_in_lobby, connection);
    //removeClientFromGame(connection); // Add this?
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
    Game *game_instance = getGameInstancebyOwner(connection);
    game_instance->run();
}

void GlobalServerState::endGame(Connection connection) {
    Game *game_instance = getGameInstancebyOwner(connection);
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

std::deque<Message> GlobalServerState::processGames() {
    std::deque<Message> outgoing;

    auto game = game_instances.begin();
    while (game != game_instances.end()) {
    // std::cout << "Game Check\n";
        switch (game->status()) {
            case GameStatus::AwaitingOutput: {
            // std::cout << "AwaitingOutput\n";
                processGameMsgs(*game, outgoing);
                game->outputSent(); // changes the status to AwaitingInput
                game++;
            }
            break;
            case GameStatus::AwaitingInput: {
            // std::cout << "AwaitingInput\n";
                std::deque<Message> player_msgs = game->playerMsgs();

                for (auto player_msg: player_msgs) {
                    Connection player = player_msg.connection;
                    if (game_input[player].first) {
                        game->registerPlayerInput(player, game_input[player].second);
                        outgoing.push_back({
                            player,
                            "Input Received, you entered: " + game_input[player].second + "\n"
                            "Waiting for other players...\n\n"
                        });
                    }
                }

                if (game->playerMsgs().size() == 0) {
                    game->run();
                }
                game++;
            }
            break;
            case GameStatus::Finished: {
            // std::cout << "GameFinished\n";
                processGameMsgs(*game, outgoing);
                outgoing.push_back({game->owner(), "\nThe game has finished!\nReturning to the lobby\n\n"});
                std::deque<Message> finalMsgs = buildMessagesForGame("\nGood game!\nYou are now back in the lobby\n\n", game->owner());
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

// FIX: Establish owner as connection in game rather than uintptr_t

// Connection
// GlobalServerState::getGameOwnerConnection(Connection connection){
//     uintptr_t gameID = clients_in_games[connection];
//     return getGameInstancebyId(gameID)->ownerID();
// }

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

bool GlobalServerState::isOngoingGame(Connection connection) {
    Game *game_instance = getGameInstancebyOwner(connection);
    return game_instance->status() != GameStatus::Finished
        && game_instance->status() != GameStatus::Created;
}

bool GlobalServerState::isValidGameInvitation(uintptr_t invitationCode) {
    Game *game_instance = getGameInstancebyInvitation(invitationCode);
    return game_instance == nullptr ? false : true;
}

void GlobalServerState::registerUserGameInput(Connection connection, std::string input) {
    game_input[connection] = {true, input};
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
GlobalServerState::buildMessagesForGame(std::string messageText, Connection connection) {
    std::deque<Message> messages;

    if (clients_in_games.find(connection) != clients_in_games.end()) {
        uintptr_t gameID = clients_in_games[connection];
        Game *game_instance = getGameInstancebyId(gameID);

        for (auto player : game_instance->players()) {
            if (!(player == connection)) {
                messages.push_back({player, messageText});
            }
        }

        // WARNING: Send to owner as well?
        // messages.push_back({game_instance->owner(), messageText});
    }

    return messages;
}

//////////////////////////////      PRIVATE METHODS     /////////////////////////////////

void GlobalServerState::processGameMsgs(Game& game, std::deque<Message>& outgoing) {
    // transform the global msg strings to message objects to be sent to the owner's screen (main screen)
    std::deque<std::string> global_msg_strings = game.globalMsgs();
    std::transform(global_msg_strings.begin(), global_msg_strings.end(), std::back_inserter(outgoing),
        [&game](auto global_msg_string) {
            return Message{ game.owner(), global_msg_string };
        }
    );

    // add the player messages to the list and flag the users requiring input (if the game is not finished)
    std::deque<Message> player_msgs = game.playerMsgs();
    if (game.status() != GameStatus::Finished) {
        for (auto msg: player_msgs) {
            game_input[msg.connection].first = false;
        }
    }
    outgoing.insert(outgoing.end(), player_msgs.begin(), player_msgs.end());
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

// compiles all the game names in ./gameconfigs into a list
// all games defined in ./gameconfigs can be "served" to users
void GlobalServerState::populateGameList() {
    gameNameList[0] = std::string("Rock Paper Scissors");
}




////////////////////////////////////////////////////////////////////////////////////////////
////////////// TEMPORARY: Manual Rock Paper Scissor Game Construction //////////////////////
#include "InterpretJson.h"
using Json = nlohmann::json;

void TEMP_ManualRpsGameConstruction(std::vector<Game>& game_instances, std::string game_name, Connection owner) {
    // TODO: use the interpreter to generate the game object from the json configurations
    // Currently manually creating Rock Paper Scissors game object
    
    //---------------LISTS--------------//

    // "constants": {
    //     "weapons": [
    //         { "name": "Rock",     "beats": "Scissors"},
    //         { "name": "Paper",    "beats": "Rock"},
    //         { "name": "Scissors", "beats": "Paper"}
    //     ]
    // },


    //Interpretr maps json info to game fields and lists
    Game g;
    std::string path = PATH_TO_JSON;
    InterpretJson j(path);
    j.interpret(g);

    //Manual
    ElementSptr setup = g.setup();
    ElementSptr constants = g.constants();
    ElementSptr variables = g.variables();
    ElementSptr per_player = g.per_player();
    ElementSptr per_audience = g.per_audience();

    std::shared_ptr<PlayerMap> players = std::make_shared<PlayerMap>(PlayerMap{});
    std::shared_ptr<std::deque<Message>> player_msgs = std::make_shared<std::deque<Message>>();
    std::shared_ptr<std::deque<std::string>> global_msgs = std::make_shared<std::deque<std::string>>();
    std::shared_ptr<std::map<Connection, std::string>> player_input = std::make_shared<std::map<Connection, std::string>>();
    std::shared_ptr<PlayerMap> audience = std::make_shared<PlayerMap>(PlayerMap{});


    //---------------RULES--------------//

    RuleVector rules{
        std::make_shared<Foreach>(
            setup->getMapElement("Rounds")->upfrom(1),
            RuleVector{
                std::make_shared<GlobalMsg>(
                    "Round {}. Choose your weapon!\n",
                    global_msgs
                ),
                std::make_shared<ParallelFor>(
                    players,
                    RuleVector{ 
                        std::make_shared<InputChoice>(
                            "{name}, choose your weapon!\n",
                            constants->getMapElement("weapons")->getSubList("name"),
                            10,
                            "weapon",
                            player_msgs,
                            player_input
                        ),
                    }
                ),
                std::make_shared<Discard>(
                    variables->getMapElement("winners"),
                    [variables](ElementSptr element) {
                        return variables->getMapElement("winners")->getSize();
                    }
                ),
                std::make_shared<Foreach>(
                    constants->getMapElement("weapons"),
                    RuleVector{
                        std::make_shared<When>(
                            std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>>{
                                std::pair<std::function<bool(ElementSptr)>,RuleVector>{
                                    [players](ElementSptr element){
                                        ElementVector player_weapons;
                                        for (auto player = players->begin(); player != players->end(); player++) {
                                            player_weapons.push_back(player->second->getMapElement("weapon"));
                                        }

                                        return player_weapons.end() == std::find_if(player_weapons.begin(), player_weapons.end(), 
                                            [element](auto player_weapon){
                                                return player_weapon->getString() == element->getMapElement("name")->getString();
                                            }
                                        );
                                    },
                                    RuleVector{
                                        std::make_shared<Extend>(
                                            variables->getMapElement("winners"),
                                            [players](ElementSptr element){
                                                ElementVector round_winners;
                                                for (auto player = players->begin(); player != players->end(); player++) {
                                                    if (player->second->getMapElement("weapon")->getString()
                                                            == element->getMapElement("beats")->getString()){
                                                        round_winners.push_back(player->second);
                                                    }
                                                }
                                                return std::make_shared<Element<ElementVector>>(round_winners);
                                            }
                                        )
                                    }
                                }
                            }
                        )
                    }
                ),
                std::make_shared<When>(
                    std::vector<std::pair<std::function<bool(ElementSptr)>,RuleVector>>{
                        std::pair<std::function<bool(ElementSptr)>,RuleVector>(
                            [variables, players](ElementSptr element){ 
                                return variables->getMapElement("winners")->getSize() == players->size(); 
                            },
                            RuleVector{
                                std::make_shared<GlobalMsg>("Tie game!\n", global_msgs)
                            }
                        ),
                        std::pair<std::function<bool(ElementSptr)>,RuleVector>(
                            [variables](ElementSptr element){ 
                                return variables->getMapElement("winners")->getSize() == 0; 
                            },
                            RuleVector{
                                std::make_shared<GlobalMsg>("Tie game!\n", global_msgs)
                            }
                        ),
                        std::pair<std::function<bool(ElementSptr)>,RuleVector>(
                            [](ElementSptr element){ 
                                return true; 
                            },
                            RuleVector{
                                std::make_shared<GlobalMsg>("Winners:\nTODO: print winner names\n", global_msgs),
                                std::make_shared<Foreach>(
                                    variables->getMapElement("winners"),
                                    RuleVector{
                                        std::make_shared<Add>(
                                            "wins", 
                                            std::make_shared<Element<int>>(1)
                                        )
                                    }
                                )
                            }
                        ),
                    }
                )
            }
        ),
        std::make_shared<Scores>(
            players,
            "wins",
            false,
            global_msgs
        )
    };


    game_instances.emplace_back(
        game_name, owner,
        /*min_players*/ 2, /*max_players*/ 4, /*audience*/ false,
        setup,
        constants, variables,
        per_player, per_audience,
        players, audience,
        rules, 
        player_msgs, global_msgs,
        player_input
    );
}