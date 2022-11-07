#include "commandHandler.h"
#include "game.h"
#include "globalState.h"
#include "InterpretJson.h"
#include "messageProcessor.h"
#include "server.h"
#include "list.h"

#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <map>


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


