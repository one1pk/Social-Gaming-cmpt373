#pragma once

#include "server.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>
#include <iostream>

////////////////////////////////////////////////////
enum UserCommand {
    EXIT,
    HELP,
    GAMES,
    CREATE,
    JOIN,
    START,
    LEAVE,
    END,
    TEXT
};

struct ProcessedMessage {
    UserCommand commandType = UserCommand::TEXT;
    std::vector<std::string> arguments;
    Connection connection;
};

class MessageProcessor {
public:
    MessageProcessor(){initializeServerCommandMap();};
    ~MessageProcessor(){};

    std::deque<ProcessedMessage> getProcessedMessages(const std::deque<Message> &incoming);

private:
    ProcessedMessage createProcessedMessage(const Message &message);
    std::vector<std::string> tokenizeMessage(std::string text);
    void initializeServerCommandMap();

    std::unordered_map<std::string, UserCommand> ServerCommandMap;
};