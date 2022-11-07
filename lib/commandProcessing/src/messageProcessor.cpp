#include "messageProcessor.h"

std::deque<ProcessedMessage>
MessageProcessor::getProcessedMessages(const std::deque<Message> &incoming) {
    std::deque<ProcessedMessage> processedMessages;

    for (auto message : incoming) {
        processedMessages.push_back(createProcessedMessage(message));
    }

    return processedMessages;
}

ProcessedMessage MessageProcessor::createProcessedMessage(const Message &message) {

    ProcessedMessage processedMessage;
    std::vector<std::string> messageTokens = tokenizeMessage(message.text);
    processedMessage.connection = message.connection;

    std::string commandString = messageTokens[0];

    if (CommandStringMap.find(commandString) != CommandStringMap.end()) {
        processedMessage.isCommand = true;
        processedMessage.commandType = CommandStringMap[commandString];

        auto eraseBegin = std::remove(messageTokens.begin(), messageTokens.end(), commandString);
        messageTokens.erase(eraseBegin, messageTokens.end());

        processedMessage.arguments = messageTokens;
    } else {
        processedMessage.input = message.text;
    }

    return processedMessage;
}

std::vector<std::string>
MessageProcessor::tokenizeMessage(const std::string &text) {

    std::vector<std::string> tokens;
    std::string token;
    std::stringstream ss(text);
    while (ss >> token) {
        tokens.push_back(token);
    }

    return tokens;
}

void MessageProcessor::initializeCommandStringMap() {
    CommandStringMap["exit"] = UserCommand::EXIT;
    CommandStringMap["help"] = UserCommand::HELP;
    CommandStringMap["games"] = UserCommand::GAMES;
    CommandStringMap["create"] = UserCommand::CREATE;
    CommandStringMap["join"] = UserCommand::JOIN;
    CommandStringMap["start"] = UserCommand::START;
    CommandStringMap["leave"] = UserCommand::LEAVE;
    CommandStringMap["end"] = UserCommand::END;
}
