#include "commandHandler.h"
#include "globalState.h"
#include "messageProcessor.h"
#include "server.h"
#include <glog/logging.h>

#include <unistd.h>
#include <chrono>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <thread>
#include <vector>
#include <map>

std::vector<User> newConnections;
std::vector<User> lostConnections;

void onConnect(User c) {
    std::cout << "New user: " << c.id << "\n";
    newConnections.push_back(c);
}

// called when a client disconnects
void onDisconnect(User c) {
    std::cout << "User lost: " << c.id << "\n";
    lostConnections.push_back(c);
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
        LOG(ERROR) << "Unable to open HTML index file:\n"
                   << htmlLocation << "\n";
        std::exit(-1);
    }
}

int main(int argc, char *argv[]) {    
    google::InitGoogleLogging(argv[0]);
    FLAGS_logtostderr = true;
    
    if (argc < 3) {
        LOG(ERROR) << "Usage:\n  " << argv[0] << " port html_response\n"
                   << "  e.g. " << argv[0] << " 4040 ./webchat.html\n";
        return 1;
    }
    LOG(INFO) << "Setting up the server...";

    /// TODO: extract the server configuration parameters from ./serverconfig.json
    // start a new session based on the configuration
    // (for now we take them as cmdline args)

    unsigned update_interval = 300;
    unsigned short port = std::stoi(argv[1]);
    Server server{port, getHTTPMessage(argv[2]), onConnect, onDisconnect};

    GlobalServerState globalState(update_interval);
    CommandHandler commandHandler(globalState);
    MessageProcessor messageProcessor;

    LOG(INFO) << "Game server is up!";

    // start listening for messages and serving content as appropriate
    while (true) {
        bool errorWhileUpdating = false;
        try {
            server.update();
        } catch (std::exception &e) {
            LOG(ERROR) << "Exception from Server update:\n"
                       << " " << e.what() << std::endl;
            errorWhileUpdating = true;
        }

        std::deque<ProcessedMessage> processedIncomingMessages = messageProcessor.getProcessedMessages(server.receive());
        std::deque<Message> outgoingMsgs = commandHandler.getOutgoingMessages(processedIncomingMessages);
        server.send(outgoingMsgs);

        std::deque<Message> outgoingGameMsgs = globalState.processGames();
        server.send(outgoingGameMsgs);

        globalState.addNewUsers(newConnections);
        std::deque<Message> outgoingDisconnectionMsgs = commandHandler.handleLostUsers(lostConnections);
        server.send(outgoingDisconnectionMsgs);

        if (errorWhileUpdating) {
            break;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(300));
    }

    return 0;
}


