#include "messageProcessor.h"

std::deque<ProcessedMessage>
MessageProcessor::getProcessedMessages(const std::deque<Message>& incoming){
    std::deque<ProcessedMessage> processedMessages;

    for(auto message : incoming){
        processedMessages.push_back(createProcessedMessage(message));
    }

    return processedMessages;
}


ProcessedMessage MessageProcessor::createProcessedMessage(const Message& message) {

    ProcessedMessage processedMessage;
    std::vector<std::string> messageTokens = tokenizeMessage(message.text);
    processedMessage.connection = message.connection;

    std::string commandString = messageTokens[0];

    if (ServerCommandMap.find(commandString) != ServerCommandMap.end()) {
        processedMessage.commandType = ServerCommandMap[commandString];
        
        auto eraseBegin = std::remove(messageTokens.begin(), messageTokens.end(), commandString);
        messageTokens.erase(eraseBegin,messageTokens.end());

        processedMessage.arguments = messageTokens;
    } else {
        processedMessage.arguments.push_back(message.text);
    }

    return processedMessage;
}

std::vector<std::string> 
MessageProcessor::tokenizeMessage(std::string text){

    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(text);
    while (ss >> token){
        tokens.push_back(token);
    }

    return tokens;
}

void
MessageProcessor::initializeServerCommandMap() {
    ServerCommandMap["exit"] = UserCommand::EXIT;
    ServerCommandMap["help"] = UserCommand::HELP;
    ServerCommandMap["games"] = UserCommand::GAMES;
    ServerCommandMap["create"] = UserCommand::CREATE;
    ServerCommandMap["join"] = UserCommand::JOIN;
    ServerCommandMap["start"] = UserCommand::START;
    ServerCommandMap["leave"] = UserCommand::LEAVE;
    ServerCommandMap["end"] = UserCommand::END;
}
