#pragma once

#include "commands.h"
#include "globalState.h"
#include "messageProcessor.h"

/**
 * This class takes in processedMessages objects that have the command type
 * It then executes that commandType and returns the response as Messages.
 */
class CommandHandler {
public:
    using commandPointer = std::unique_ptr<Command>;

    CommandHandler(GlobalServerState &globalState) : globalState{globalState} { initializeMaps(); }
    ~CommandHandler() {};

    /**
     * Adds new command objects to the commandMap
     */
    void registerCommand(UserCommand, commandPointer);

    /**
     * Returns the messages after executing the incoming commands represented by processedMessages
     */
    std::deque<Message> getOutgoingMessages(const std::deque<ProcessedMessage> &incomingProcessedMessages);

    /**
     * Handles any lost users that didn't disconenct properly 
     * calls commands *leave*, *exit*, & *end* artficially as required
     * NOTE: clears the users vector after its done
     */
    std::deque<Message> handleLostUsers(std::vector<User> &users); 

private:
    GlobalServerState &globalState;
    std::deque<Message> outgoing;
    std::unordered_map<UserCommand, commandPointer> commandMap;      // Maps command names to command objects
    std::unordered_map<CommandResult, std::string> commandResultMap; // Maps command results to feedback strings

    void initializeMaps();
    void initializeCommandMap();
    void initializeCommandResultMap();
    void registerCommandImpl();

    /**
     * Main function responsible for handling execution of a single command type.
     * Calls the execute method of command object in the commandMap
     */
    CommandResult executeCommand(ProcessedMessage &processedMessage);

    /**
     * Boradcast lobby message to all users in lobby
     */
    void broadcastLobbyMessage(ProcessedMessage &processedMessage);
};