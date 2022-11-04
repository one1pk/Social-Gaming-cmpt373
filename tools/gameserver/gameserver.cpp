#include "game.h"
#include "interpreter.h"
#include "server.h"
#include "globalState.h"
#include "messageProcessor.h"
#include "commandHandler.h"

#include <algorithm>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <unistd.h>
#include <unordered_set>
#include <vector>


std::unique_ptr<GlobalServerState> globalState = std::make_unique<GlobalServerState>();

// called when a client connects

// FIX: Resolve stuff here, REMOVE OR ADD CONNECTION FROM GLOBAL STATE
void onConnect(Connection c) {
    std::cout << "New connection: " << c.id << "\n";
    globalState->addConnection(c);
}

// called when a client disconnects
void onDisconnect(Connection c) {
    std::cout << "Connection lost: " << c.id << "\n";
    globalState->disconnectConnection(c);
}

// extracts the port number from ./serverconfig.json
unsigned short getPort() {
    /** STUB **/
    return 4040;
}

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
    std::unique_ptr<MessageProcessor> messageProcessor = std::make_unique<MessageProcessor>();
    std::unique_ptr<CommandHandler> commandHandler = std::make_unique<CommandHandler>(*globalState);

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

        std::deque<ProcessedMessage> processedIncomingMessages = messageProcessor->getProcessedMessages(server.receive());

        std::deque<Message> outgoing = commandHandler->getOutgoingMessages(processedIncomingMessages);

        server.send(outgoing);

        if (errorWhileUpdating) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }

    return 0;
}