#include "client.h"
#include "ChatWindow.h"
#include <glog/logging.h>

#include <iostream>


/*
When a user connects to the game server, they may either join a game
or create a new game amongst those served.
When a new game is created, it may be configured by the creating player,
who is the “owner” of the game session and will have admin controls over it.
The owner may configure the game and create it, 
after which they will receive an “invite code”.
Other players connecting to the game server may join the game by specifying the invite code.
Note, the owner of the game is not a player.
If they wish to play the game, they must also join.
*/
std::string WELCOME_MESSAGE = (
    "                          Welcome to Social Gaming!\n\n"
        "You are now in the main lobby, here you can chat with other players in the lobby\n"   
        "  * To view the available games enter: games\n" 
        "  * To create a new game from the options enter: create <game_index>\n"
        "  * To join a game with an invitation code enter: join <invitation_code>\n"
        "  * To exit the server enter: exit\n"
        "  * To display help information again enter: help\n\n");

int main(int argc, char* argv[]) {
    google::InitGoogleLogging(argv[0]);
    
    // verify that the cmdline arguments provided describe a game server
    // and connect to the server

    if (argc < 3) {
        std::cerr << "Usage: \n  " << argv[0] << " <ip address> <port> <your game name>\n"
                << "  e.g. " << argv[0] << " localhost 4002 John\n";
        return 1;
    }
    Client client{argv[1], argv[2]};

    bool done = false;
    auto onTextEntry = [&done, &client] (std::string text) {
        client.send(text);
        if ("exit" == text) {
            done = true;
        }
    };
    ChatWindow chatWindow(onTextEntry);

    chatWindow.displayText(WELCOME_MESSAGE);

    while (!done && !client.isDisconnected()) {
        try {
            client.update();
        } catch (std::exception& e) {
            chatWindow.displayText("Exception from Client update:");
            chatWindow.displayText(e.what());
            done = true;
        }

        auto response = client.receive();
        if (!response.empty()) {
            chatWindow.displayText(response);
        }
        chatWindow.update();
    }


    return 0;
}