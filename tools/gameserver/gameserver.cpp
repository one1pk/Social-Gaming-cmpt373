#include "commandHandler.h"
#include "game.h"
<<<<<<< HEAD
#include "globalState.h"
#include "interpreter.h"
#include "messageProcessor.h"
=======
#include "InterpretJson.h"
>>>>>>> 19b7927 (Interpret part of json and add test)
#include "server.h"
#include "list.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
<<<<<<< HEAD
#include <map>
=======
using namespace ns;
using namespace std;
>>>>>>> 19b7927 (Interpret part of json and add test)


GlobalServerState globalState;

// FIX: Resolve stuff here, REMOVE OR ADD CONNECTION FROM GLOBAL STATE
void onConnect(Connection c) {
    std::cout << "New connection: " << c.id << "\n";
    globalState.addConnection(c);
}

// called when a client disconnects
void onDisconnect(Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
    // globalState->disconnectConnection(c);
}

// extracts the port number from ./serverconfig.json
unsigned short getPort() {
    /** STUB **/
    return 4040;
}

// #include <unistd.h>
// std::string getHTTPMessage(const char *htmlLocation) {
//     if (access(htmlLocation, R_OK) != -1) {
//         std::ifstream infile{htmlLocation};
//         return std::string{std::istreambuf_iterator<char>(infile),
//                            std::istreambuf_iterator<char>()};
//     } else {
//         std::cerr << "Unable to open HTML index file:\n"
//                   << htmlLocation << "\n";
//         std::exit(-1);
//     }
// }


int main(int argc, char *argv[]) {    
    if (argc < 3) {
        std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
                  << "  e.g. " << argv[0] << " 4040 ./webchat.html\n";
        return 1;
    }
    std::cout << "Setting up the server...\n";

    /// TODO: extract the server configuration parameters from ./serverconfig.json
    // start a new session based on the configuration
    // (for now we take them as cmdline args)

    unsigned short port = std::stoi(argv[1]);

    Server server{port, ""/*getHTTPMessage(argv[2])*/, onConnect, onDisconnect};
    MessageProcessor messageProcessor;
    CommandHandler commandHandler(globalState);

    std::cout << "Game server is up!\n";

    // start listening for messages and serving content as appropriate
    while (true) {
        bool errorWhileUpdating = false;
        try {
            server.update();
        } catch (std::exception &e) {
            std::cerr << "Exception from Server update:\n"
                      << " " << e.what() << "\n\n";
            errorWhileUpdating = true;
        }

        std::deque<ProcessedMessage> processedIncomingMessages = messageProcessor.getProcessedMessages(server.receive());

        std::deque<Message> outgoingMsgs = commandHandler.getOutgoingMessages(processedIncomingMessages);
        server.send(outgoingMsgs);

        std::deque<Message> outgoingGameMsgs = globalState.processGames();
        server.send(outgoingGameMsgs);

        if (errorWhileUpdating) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}


<<<<<<< HEAD
=======
bool processMessages(Server &server) {
    bool shutdown = false;
    std::deque<Message> outgoing;
    std::ostringstream lobbyMsgStream;
    const std::deque<Message> &incoming = server.receive();

    for (auto &message : incoming) {
        if (message.connection.in_game) { // an in-game message //
            Game* game_instance = getGameInstance(message.connection.gameID);

            if (message.text == "leave") {
                HandleLeave(message, outgoing, server, game_instance);
            } else if (message.text == "end") {
                HandleEnd(message, outgoing, server, game_instance);
            } else if (message.text == "join" && message.connection.id == game_instance->ownerID()) {
                HandleOwnerJoin(message, outgoing, server, game_instance);
            } else if (message.text == "start" && message.connection.id == game_instance->ownerID()){
                HandleStart(message, outgoing, server, game_instance);
            } else {
                // currently all in-game messages are broadcasted to all players
                // TODO: Handle input and output for a game
                // will need to integrate with the Game object and the input/output game rules
                
                if (message.connection.id == game_instance->ownerID() && !game_instance->hasPlayer(message.connection)) {
                    // if owner hasnt joined, don't broadcast their message
                    continue;
                }
                
                std::ostringstream ingame_message;
                ingame_message << message.connection.id << "> " << message.text << "\n";

                for (auto &player : game_instance->players()) {
                    outgoing.push_back({player, ingame_message.str()});
                }
            }
        } else { // a lobby message //

            if (message.text == "exit") {
                server.disconnect(message.connection);
            } else if (message.text == "shutdown") {
                // TODO: Remove this command
                std::cout << "Shutting down.\n";
                shutdown = true;

            } else if (message.text == "games") {
                // games list request, respond with a list of game names to requestor
                outgoing.push_back({message.connection, gamesList()});
            } else if (message.text.substr(0, 7) == "create ") {
                HandleCreate(message, outgoing, server);
            } else if (message.text.substr(0, 5) == "join ") {
                HandleJoin(message, outgoing, server);
            } else if (message.text == "help") {
                // display list of existing commands
                outgoing.push_back({message.connection, HELP_INFORMATION});
            } else {
                // anything else is a regular lobby message that is broadcasted to everyone
                lobbyMsgStream << message.connection.id << "> " << message.text << "\n";
            }
        }
    }

    // broadcast the lobby messages to everyone in the lobby
    for (auto client : clients_in_lobby) {
        outgoing.push_back({client, lobbyMsgStream.str()});
    }

    server.send(outgoing);

    return shutdown;
}

void HandleJoin(Message message, std::deque<Message> &outgoing, Server &server) {
    // validate invitation code
    std::string game_index_string = message.text.substr(5, message.text.size() - 5);
    if (!game_index_string.empty()) {

        int game_id = std::stoi(message.text.substr(5, message.text.size() - 5));
        Game *game_instance = getGameInstance(game_id);

        if (game_instance) {
            // join the game, remove player from lobby, change player's connection info
            game_instance->addPlayer(message.connection);
            removeConnectionFromList(message.connection, clients_in_lobby);
            server.setGameIdentity(message.connection, game_instance->id());

            // return a confirmation message to the player
            std::stringstream confirmation;
            confirmation << "You have joined a " << game_instance->getName() << " game!\n"
                         << "There are currently " << game_instance->numPlayers() << " players\n"
                         << "waiting for the game owner to start the game...\n"
                         << "Meanwhile, you can chat in the game lobby\n\n";
            outgoing.push_back({message.connection, confirmation.str()});

            // return a notification message to the game owner
            // WARNING: Check errors here for non-valid owner id
            std::stringstream notification;
            notification << "\nUser " << message.connection.id << " joined the game\n"
                         << "Player Count: " << game_instance->numPlayers() << "\n\n";

            auto owner_ID = game_instance->ownerID();
            auto ownerConnection = std::find_if(clients.begin(), clients.end(), [owner_ID](auto client) { return client.id == owner_ID; });
            outgoing.push_back({*ownerConnection, notification.str()});

            // return a notification message to each game player
            for (auto &player : game_instance->players()) {
                if (player == message.connection || player == *ownerConnection)
                    continue;
                outgoing.push_back({player, notification.str()});
            }
        } else {
            outgoing.push_back({message.connection, "Please enter a valid invitation code\n"});
        }
    } else {
        outgoing.push_back({message.connection, "Incorrect format. To join a game, please enter: join <invitation code>\n"});
    }
}

void HandleOwnerJoin(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance) {

    // add the owner to their game if not already in it 
    if (!game_instance->hasPlayer(message.connection)) {
        game_instance->addPlayer(message.connection);

        // return a confirmation message
        std::stringstream confirmation;
        confirmation << "You have joined your own game!\n"
                        << "Start the game when you are ready\n"
                        << "Meanwhile, you can chat in the game lobby\n\n";
        outgoing.push_back({message.connection, confirmation.str()});

        // return a notification message to each game player
        for (auto &player : game_instance->players()) {
            if (player == message.connection)
                continue;
            outgoing.push_back({player, "The game owner has joined the game!\n"});
        }
    }
}

void HandleCreate(Message message, std::deque<Message> &outgoing, Server &server) {
    // Validate game index
    std::string game_index_string = message.text.substr(7, message.text.size() - 7);
    if (!game_index_string.empty()) {
        int game_index = std::stoi(game_index_string);
        if (game_index < (int)game_names.size()) {
            // start a new game, remove owner from lobby, change owner's connection info
            // and return confirmation and invitation code to owner
            constructGame(game_names[game_index], message.connection.id);
            removeConnectionFromList(message.connection, clients_in_lobby);
            server.setGameIdentity(message.connection, game_instances.back().id());
            std::stringstream confirmation;
            confirmation << "Game " << game_names[game_index] << " created successfully\n"
                         << "Invitation code: " << game_instances.back().id() << "\n"
                         << "Enter \"start\" when you are ready to start the game\n\n";
            outgoing.push_back({message.connection, confirmation.str()});
        } else {
            outgoing.push_back({message.connection, "Please enter a valid game index. To list all games, enter: games\n"});
        }
    } else {
        outgoing.push_back({message.connection, "Incorrect Format! To create a game, please enter: create <game index>\n"});
    }
}

void HandleStart(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance) {
    if (game_instance->isOngoing()) return;

    // reply with confirmation to game owner
    outgoing.push_back({message.connection, "Game Started\n"});

    // send notification to game players
    for (auto &player : game_instance->players()) {
        if (player.id == game_instance->ownerID())
            continue;
        outgoing.push_back({player, "The owner has started the game!\n\n"});
    }

    game_instance->start();
}

void HandleLeave(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance) {
    // non-owners can leave
    if (message.connection.id == game_instance->ownerID()) {
        outgoing.push_back({message.connection, "Owners cannot leave the game they create. To end the game, please enter: end\n"});
    } else {
        game_instance->removePlayer(message.connection);
        server.setGameIdentity(message.connection, 0);
        outgoing.push_back({message.connection, "You have successfully left the game!\n\n"});
        clients_in_lobby.push_back(message.connection);
        
        // Notify everyone else and owner
        std::stringstream notification;
            notification << "\nUser " << message.connection.id << " left the game\n"
                         << "Player Count: " << game_instance->numPlayers() << "\n\n";
        for (auto &player : game_instance->players()) {
            if (player == message.connection || player.id == game_instance->ownerID())
                continue;
            outgoing.push_back({player, notification.str()});
        }
        auto owner_ID = game_instance->ownerID();
        auto ownerConnection = std::find_if(clients.begin(), clients.end(), [owner_ID](auto client) { return client.id == owner_ID; });
        outgoing.push_back({*ownerConnection, notification.str()});
    }
}

void HandleEnd(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance) {
    // owner ends game session
    if (message.connection.id == game_instance->ownerID()) {
        outgoing.push_back({message.connection, "Game successfully ended! Entering the lobby now!\n\n"});
        // Adds owner back to lobby
        server.setGameIdentity(message.connection, 0);
        clients_in_lobby.push_back(message.connection);
        
        // return a notification message to each game player and send them to lobby
        std::string player_notification = "The game owner has ended the game. You will be transferred to the lobby.\n\n";
        for (auto &player : game_instance->players()) {
            if (player == message.connection)
                continue;
            outgoing.push_back({player, player_notification});
            clients_in_lobby.push_back(player);
            server.setGameIdentity(player, 0);
        }

        // TODO: CHECK FOR MEMORY LEAKS IN GAME
        removeGameInstance(game_instance->id());

    } else {
        outgoing.push_back({message.connection, "Only the game owner can end the game. To leave the game, please enter: leave\n"});
    }
}
>>>>>>> 19b7927 (Interpret part of json and add test)
