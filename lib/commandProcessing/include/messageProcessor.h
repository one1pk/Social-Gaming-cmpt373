#pragma once

#include "server.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>

/**
 * Possible commands that a user may enter
 */
enum UserCommand {
    EXIT,
    HELP,
    GAMES,
    CREATE,
    JOIN,
    START,
    LEAVE,
    END,
    CHAT
};

/**
 * Object that has been created by processing raw messages from the users. Contains:
 * - Connection : unique user identifier
 * - commandType : Type of command (by default a CHAT)
 * - Arguments : The arguments provided for that particular command as tokens
 */
struct ProcessedMessage {
    UserCommand commandType = UserCommand::CHAT;
    std::vector<std::string> arguments;
    Connection connection;
};

/**
 * Message Processor interface manipulates the text strings contained in the messages and maps
 * them to respective UserCommand enums for further use by command handler.
 * It also separates the arguments provided by storing them as a vector of string tokens.
 */
class MessageProcessor {
public:
    MessageProcessor() { initializeCommandStringMap(); };
    ~MessageProcessor(){};

    std::deque<ProcessedMessage> getProcessedMessages(const std::deque<Message> &incoming);

private:
    ProcessedMessage createProcessedMessage(const Message &message);
    std::vector<std::string> tokenizeMessage(const std::string &text);
    void initializeCommandStringMap();

    std::unordered_map<std::string, UserCommand> CommandStringMap;
};