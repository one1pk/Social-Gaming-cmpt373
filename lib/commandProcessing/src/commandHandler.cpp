#include "commandHandler.h"

void CommandHandler::initializeMaps() {
    initializeCommandMap();
    initializeCommandResultMap();
}

std::deque<Message>
CommandHandler::getOutgoingMessages(const std::deque<ProcessedMessage> &incomingProcessedMessages) {

    outgoing.clear();

    for (auto processedMessage : incomingProcessedMessages) {
        commandResult commandResult = executeCommand(processedMessage);
        if (commandResult != commandResult::SUCCESS) {
            outgoing.push_back({processedMessage.connection, commandResultMap[commandResult]});
        }
    }

    return outgoing;
}

commandResult
CommandHandler::executeCommand(ProcessedMessage &processedMessage) {
    return commandMap[processedMessage.commandType]->execute(processedMessage);
}

void CommandHandler::registerCommand(UserCommand userCommand, commandPointer commandPointer) {
    commandMap[userCommand] = std::move(commandPointer);
}

void CommandHandler::initializeCommandMap() {

    registerCommand(UserCommand::CREATE, std::make_unique<CreateGameCommand>(globalState, outgoing));
    registerCommand(UserCommand::START, std::make_unique<StartGameCommand>(globalState, outgoing));
    registerCommand(UserCommand::JOIN, std::make_unique<JoinGameCommand>(globalState, outgoing));
    registerCommand(UserCommand::LEAVE, std::make_unique<LeaveGameCommand>(globalState, outgoing));
    registerCommand(UserCommand::END, std::make_unique<EndGameCommand>(globalState, outgoing));
    registerCommand(UserCommand::HELP, std::make_unique<ListHelpCommand>(globalState, outgoing));
    registerCommand(UserCommand::GAMES, std::make_unique<ListGamesCommand>(globalState, outgoing));
    registerCommand(UserCommand::CHAT, std::make_unique<ChatCommand>(globalState, outgoing));
    registerCommand(UserCommand::EXIT, std::make_unique<ExitServerCommand>(globalState, outgoing));
}

void CommandHandler::initializeCommandResultMap() {
    commandResultMap[commandResult::ERROR_INCORRECT_COMMAND_FORMAT] = "Incorrect Command Format.\nPlease enter: <command> <arguments>. To list all commands, enter: help\n\n";
    commandResultMap[commandResult::ERROR_INVALID_GAME_INDEX] = "Invalid Game Index.\nTo list all available games, enter: games\n\n";
    commandResultMap[commandResult::ERROR_INVALID_INVITATION_CODE] = "Invalid invitation code entered: No game found.\n\n";
    commandResultMap[commandResult::ERROR_INVALID_START_GAME_COMMAND] = "The game has already started.\n\n";
    commandResultMap[commandResult::ERROR_NOT_AN_OWNER] = "Only an owner can start or end the game.\n\n";
    commandResultMap[commandResult::ERROR_OWNER_CANNOT_LEAVE] = "Owner cannot leave their game. To end game for all players, enter : end\n\n";
    commandResultMap[commandResult::ERROR_OWNER_CANNOT_JOIN_FROM_SAME_DEVICE] = "Owner cannot join the game from same window. Please open up a new window to join the game!\n\n";
    commandResultMap[commandResult::ERROR_INVALID_COMMAND] = "Invalid Command!\n\n";
    commandResultMap[commandResult::ERROR_GAME_NOT_STARTED] = "The game has not been started yet!\n\n";

    commandResultMap[commandResult::SUCCESS_GAME_CREATION] = "Game Successfully Created with invitation Code : \nPlease enter \"start\" to start the game.\n\n";
    commandResultMap[commandResult::SUCCESS_GAME_JOIN] = "Game successfully joined. Waiting for the owner to start\n\n";
    commandResultMap[commandResult::SUCCESS_GAME_START] = "Game Started!\n\n";
    commandResultMap[commandResult::SUCCESS_GAME_END] = "Game Ended. All players have been moved to server lobby.\n\n";
    commandResultMap[commandResult::SUCCESS_GAME_LEAVE] = "Game Successfully left.\n\n";

    commandResultMap[commandResult::STRING_SERVER_HELP] =
        "\n"
        "  * To view the available games enter: games\n"
        "  * To create a new game from the options enter: create <game_index>\n"
        "  * To join a game with an invitation code enter: join <invitation_code>\n"
        "  * To exit the server enter: exit\n"
        "  * To display help information again enter: help\n\n";
    commandResultMap[commandResult::STRING_INGAME_PLAYER_HELP] =
        "\n"
        "  * To leave the game enter: leave\n"
        "  * To exit the server enter: exit\n\n";
    commandResultMap[commandResult::STRING_INGAME_OWNER_HELP] =
        "\n"
        "  * To start the game enter: start\n"
        "  * To end the game enter: end\n"
        "  * To exit the server enter: exit\n\n";
}