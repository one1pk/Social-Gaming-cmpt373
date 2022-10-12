#include "server.h"
#include "game.h"
#include "interpreter.h"

#include <algorithm>
#include <unistd.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <chrono>
#include <thread>
#include <string>

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
Game* getGameInstance(uintptr_t gameID) {
    auto game_it = std::find_if(std::begin(game_instances), std::end(game_instances), [=](auto &game) {
        return game.id() == gameID;
    });

    if (game_it == std::end(game_instances))
        return nullptr;
    else
        return &*game_it;
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


struct ProcessedMessages {
    std::deque<Message> outgoing;
    bool shouldShutdown;
};

// processes all incoming messages received from clients since the last Server::update()
// distinguishes control signals and commands and responds appropriately
// returns a list of Message objects to be sent by the server and a shutdown signal (if any was received)
ProcessedMessages processMessages(Server& server) {
    bool shutdown = false;
    std::deque<Message> outgoing;
    std::ostringstream lobbyMsgStream;
    const std::deque<Message>& incoming = server.receive();

    for (auto& message : incoming) {
        if (message.in_game) { // an in-game message //

            if (message.text == "leave") {
                // leave current game (go back to the lobby)
                // non-owners only

                if (message.connection.id == getGameInstance(message.gameID)->ownerID()) {
                    // reply "owners can't leave the game they create, to end the game enter: end"

                } else {
                    // remove player from game
                    // reply with a confirmation
                }
            } else if (message.text == "end") {
                // end current game (all players go back to the lobby)
                // owners only

                if (message.connection.id == getGameInstance(message.gameID)->ownerID()) {
                    // end game session
                    // broadcast to all players that the owner has ended the game

                } else {
                    // reply "only the game owner can end the game, to leave enter: leave"
                }
            } else {
                // anything else is either game input or in-game chat messages (perhaps we don't need the latter)
                // game inputs are tied to the game rules (still need to integrate)
                // in-game chat is broadcasted to all game players

            }
        } else { // a lobby message //

            if (message.text == "exit") {
                // exit the server
                server.disconnect(message.connection);

            } else if (message.text == "shutdown") {
                // shutdown the server (currently any user can shutdown the server)
                std::cout << "Shutting down.\n";
                shutdown = true;

            } else if (message.text == "games") {
                // games list request, respond with a list of game names
                // this is a private response - goes only to the client that requested it
                outgoing.push_back({message.connection, gamesList()});

            } else if (message.text.substr(0, 6) == "start ") {
                //** needs error checking **//

                // start a new game
                int game_index = std::stoi(message.text.substr(6, message.text.size()-6));
                constructGame(game_names[game_index], message.connection.id);
                removeConnectionFromList(message.connection, clients_in_lobby);

                // return a confirmation and an invitation code to the game owner
                std::stringstream confirmation;
                confirmation << "Game " << game_names[game_index] << " created successfully\n"
                             << "Invitation code: " << game_instances.back().id() << "\n\n";

                outgoing.push_back({message.connection, confirmation.str()});


                // allow user to configure any configurable fields for the game

            
            } else if (message.text.substr(0, 5) == "join ") {
                //** needs error checking **//

                // join an existing game
                int game_id = std::stoi(message.text.substr(5, message.text.size()-5));
                Game* game_instance = getGameInstance(game_id);

                // validate invitation code
                if (game_instance) {
                    game_instance->addPlayer(message.connection);
                    removeConnectionFromList(message.connection, clients_in_lobby);

                    // return a confirmation message to the player
                    std::stringstream confirmation;
                    confirmation << "You have joined a " << game_instance->name() << " game!\n"
                                 << "there are currently " << game_instance->numPlayers() << " players\n"
                                 << "waiting for the game owner to start the game...\n"
                                 << "meanwhile, you can chat in the game lobby\n\n";

                    outgoing.push_back({message.connection, confirmation.str()});
                    
                    // return a notification message to the game owner
                    std::stringstream notification;
                    notification << "User " << message.connection.id << " joined the game\n"
                                 << "there are currently " << game_instance->numPlayers() << " players\n\n";

                    outgoing.push_back({game_instance->ownerID(), notification.str()});

                    // return a notification message to each game player
                    for (auto &player: game_instance->players()) {
                        if (player == message.connection)
                            continue;
                        outgoing.push_back({player, notification.str()});
                    }
                }

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
    return ProcessedMessages{outgoing, shutdown};
}


std::deque<Message> buildOutgoing(const std::string& log) {
    std::deque<Message> outgoing;
    for (auto client : clients) {
        outgoing.push_back({client, log, false, 0});
    }
    return outgoing;
}

std::string
getHTTPMessage(const char* htmlLocation) {
  if (access(htmlLocation, R_OK ) != -1) {
    std::ifstream infile{htmlLocation};
    return std::string{std::istreambuf_iterator<char>(infile),
                       std::istreambuf_iterator<char>()};

  } else {
    std::cerr << "Unable to open HTML index file:\n"
              << htmlLocation << "\n";
    std::exit(-1);
  }
}


int main(int argc, char* argv[]) {
    if (argc < 3) {
        std::cerr << "Usage:\n  " << argv[0] << " <port> <html response>\n"
                << "  e.g. " << argv[0] << " 4002 ./webchat.html\n";
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
        } catch (std::exception& e) {
            std::cerr << "Exception from Server update:\n"
                      << " " << e.what() << "\n\n";
            errorWhileUpdating = true;
        }

        auto [outgoing, shutdownSignal] = processMessages(server);
        server.send(outgoing);

        if (shutdownSignal || errorWhileUpdating) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}