#pragma once

#include "commands.h"
#include "globalState.h"
#include "messageProcessor.h"

enum class commandOutput {
    ERROR_INCORRECT_COMMAND_FORMAT,
    ERROR_INVALID_GAME_INDEX,
    ERROR_INVALID_INVITATION_CODE,
    ERROR_INVALID_START_GAME_COMMAND,
    ERROR_NOT_AN_OWNER,
    ERROR_OWNER_CANNOT_LEAVE,
    ERROR_INVALID_COMMAND,

    SUCCESS_GAME_CREATION,
    SUCCESS_GAME_JOIN,
    SUCCESS_GAME_START,
    SUCCESS_GAME_END,
    SUCCESS_GAME_LEAVE,

    STRING_HELP,

};

/**
 * This class takes in processedMessages objects that have the command type
 * it them executes that commandType and returns the response as Message for each command.
 */
class CommandHandler {
public:
    using commandPointer = std::unique_ptr<Command>;

    CommandHandler(GlobalServerState &globalState) : globalState{globalState} { initializeMaps(); }
    ~CommandHandler(){};

    void initializeMaps();
    std::deque<Message> getOutgoingMessages(const std::deque<ProcessedMessage> &incomingProcessedMessages);

private:
    GlobalServerState &globalState;
    std::deque<Message> outgoing;
    std::unordered_map<UserCommand, commandPointer> commandMap;
    std::unordered_map<commandResult, std::string> commandResultMap;

    void initializeCommandMap();
    void initializeCommandResultMap();
    commandResult executeCommand(ProcessedMessage &processedMessage);
};