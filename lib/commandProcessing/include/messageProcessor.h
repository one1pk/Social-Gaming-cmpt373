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
    USERNAME,
};

/**
 * Object that has been created by processing raw messages from the users. Contains:
 * - isCommand : whether the user message was a command or not 
 * - User : unique user identifier
 * - commandType : Type of command (by default a INPUT)
 * - Arguments : The arguments provided for that particular command as tokens
 * - input : IF NOT COMMAND, the message text 
 */
struct ProcessedMessage {
    bool isCommand = false;
    User user;
    UserCommand commandType;
    std::vector<std::string> arguments;
    std::string input;
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