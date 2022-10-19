#include "game.h"
#include "interpreter.h"
#include "server.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <vector>

/*
game server that can be configured by the person running it
to allow games to be played.
The configuration of the server will be specified in JSON.
Similarly, the specification for a single game will be in JSON.
A game specification will define the state that a game must maintain
as well as the rules of the game.
You may think of the specifications as being written in a domain specific
programming language that your game server will interpret.
*/

std::vector<Connection> clients;
std::vector<Connection> clients_in_lobby;
// std::vector<std::vector<Connection>> clients_in_games;
std::vector<std::string> game_names;
std::vector<Game> game_instances;

std::string HELP_INFORMATION = ("\n\n                       Displaying Help Information\n"
                                "  * To view the available games enter: games\n"
                                "  * To create a new game from the options enter: create <game_index>\n"
                                "  * To join a game with an invitation code enter: join <invitation_code>\n"
                                "  * To exit the server enter: exit\n\n");

void removeConnectionFromList(Connection c, std::vector<Connection> &list) {
    auto eraseBegin = std::remove(std::begin(list), std::end(list), c);
    list.erase(eraseBegin, std::end(list));
}

// called when a client connects
void onConnect(Connection c) {
    std::cout << "New connection: " << c.id << "\n";
    clients.push_back(c);
    clients_in_lobby.push_back(c);
}

// called when a client disconnects
void onDisconnect(Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
    // TODO: If owner, end game and send players back to lobby
    removeConnectionFromList(c, clients);
    removeConnectionFromList(c, clients_in_lobby);
}

// extracts the port number from ./serverconfig.json
unsigned short getPort() {
    /** STUB **/
    return 4040;
}

// compiles all the game names in ./gameconfigs into a list
// all games defined in ./gameconfigs can be "served" to users
void createAvailableGamesList() {
    /** STUB **/
    // use the interpreter to parse the jsons for names
    game_names.push_back("rock_paper_scissors");
}

// creates a game object corresponding to the game configuration in ./gameconfigs called <game_name>.json
void constructGame(std::string game_name, uintptr_t ownerID) {
    /** STUB **/
    // use the interpreter to generate the game object from the json configurations
    game_instances.emplace_back(game_name, ownerID);
}

// returns a pointer to the game instance with the given gameID
Game *getGameInstance(uintptr_t gameID) {
    auto game_it = std::find_if(std::begin(game_instances), std::end(game_instances), [=](auto &game) {
        return game.id() == gameID;
    });

    if (game_it == std::end(game_instances))
        return nullptr;
    else
        return &*game_it;
}

void removeGameInstance(uintptr_t gameID){
    auto end_it = std::remove_if(game_instances.begin(), game_instances.end(), [gameID](auto &game){
        return game.id() == gameID;
    });

    game_instances.erase(end_it, game_instances.end());
}

// returns a list of game names and indexes in string form
// of the format:
// [0] rock, paper, scissors
// [1] another game ...
// the game indicies will match their index in the game_names vector
std::string gamesList() {
    std::stringstream list;
    list << "Games Available:\n";
    for (size_t i = 0; i < game_names.size(); i++) {
        list << " [" << i << "] " << game_names[i] << "\n";
    }
    return list.str();
}

// processes all incoming messages received from clients since the last Server::update()
// responds to control messages and commands and broadcasts chat messages
// invokes the Server::send() to send the appropriate messages back to the clients
// returns a shutdown signal (if any was received)
bool processMessages(Server &server);

std::string getHTTPMessage(const char *htmlLocation) {
    if (access(htmlLocation, R_OK) != -1) {
        std::ifstream infile{htmlLocation};
        return std::string{std::istreambuf_iterator<char>(infile),
                           std::istreambuf_iterator<char>()};
    } else {
        std::cerr << "Unable to open HTML index file:\n"
                  << htmlLocation << "\n";
        std::exit(-1);
    }
}

int main(int argc, char *argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
                  << "  e.g. " << argv[0] << " 4040 ./webchat.html\n";
        return 1;
    }
    std::cout << "Setting up the server...\n";

    // extract the server configuration parameters from ./serverconfig.json
    // start a new session based on the configuration
    // (for now we take them as cmdline args)

    unsigned short port = std::stoi(argv[1]);
    Server server{port, getHTTPMessage(argv[2]), onConnect, onDisconnect};

    std::cout << "Game server is up!\n";

    createAvailableGamesList();

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

        bool shutdownSignal = processMessages(server);

        if (shutdownSignal || errorWhileUpdating) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}

void HandleJoin(Message message, std::deque<Message> &outgoing, Server &server);
void HandleOwnerJoin(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance);
void HandleCreate(Message message, std::deque<Message> &outgoing, Server &server);
void HandleStart(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance);
void HandleLeave(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance);
void HandleEnd(Message message, std::deque<Message> &outgoing, Server &server, Game* game_instance);

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
            confirmation << "You have joined a " << game_instance->name() << " game!\n"
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