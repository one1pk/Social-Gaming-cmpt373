#include "commandHandler.h"

void
CommandHandler::initializeMaps(){
    initializeCommandMap();
    initializeCommandResultMap();
}

std::deque<Message> 
CommandHandler::getOutgoingMessages(const std::deque<ProcessedMessage>& incomingProcessedMessages){

    outgoing.clear();

    for(auto processedMessage : incomingProcessedMessages){
        commandResult commandResult = executeCommand(processedMessage);
        if(commandResult!=commandResult::SUCCESS){
            outgoing.push_back({processedMessage.connection, commandResultMap[commandResult]});
        }
    }

    return outgoing;
}

commandResult 
CommandHandler::executeCommand(ProcessedMessage &processedMessage){
    return commandMap[processedMessage.commandType]->execute(processedMessage.connection, processedMessage.arguments, globalState, outgoing);
}


void
CommandHandler::initializeCommandMap(){
    commandMap[UserCommand::CREATE] = std::move(std::make_unique<CreateGameCommand>());
    commandMap[UserCommand::START] = std::move(std::make_unique<StartGameCommand>());
    commandMap[UserCommand::JOIN] = std::move(std::make_unique<JoinGameCommand>());
    commandMap[UserCommand::LEAVE] = std::move(std::make_unique<LeaveGameCommand>());
    commandMap[UserCommand::END] = std::move(std::make_unique<EndGameCommand>());
    commandMap[UserCommand::HELP] = std::move(std::make_unique<ListHelpCommand>());
    commandMap[UserCommand::GAMES] = std::move(std::make_unique<ListGamesCommand>());
    // commandMap[UserCommand::EXIT] = std::move(std::make_unique<ExitServerCommand>());
    commandMap[UserCommand::TEXT] = std::move(std::make_unique<TextCommand>());

}

void 
CommandHandler::initializeCommandResultMap(){
    commandResultMap[commandResult::ERROR_INCORRECT_COMMAND_FORMAT] = "Incorrect Command Format.\n Please enter: <command> <arguments>. To list all commands, enter: help\n";
    commandResultMap[commandResult::ERROR_INVALID_GAME_INDEX] = "Invalid Game Index.\n To list all available games, enter: games\n";
    commandResultMap[commandResult::ERROR_INVALID_INVITATION_CODE] = "Invalid invitation code entered: No game found.\n";
    commandResultMap[commandResult::ERROR_INVALID_START_GAME_COMMAND] = "The game has already started.\n";
    commandResultMap[commandResult::ERROR_NOT_AN_OWNER] = "Only an owner can start or end the game.\n";
    commandResultMap[commandResult::ERROR_OWNER_CANNOT_LEAVE] = "Owner cannot leave their game. To end game for all players, enter : end\n";
    commandResultMap[commandResult::ERROR_INVALID_COMMAND] = "Invalid Command!\n";

    commandResultMap[commandResult::SUCCESS_GAME_CREATION] = "Game Successfully Created with invitation Code : \nPlease enter \"start\" to start the game.\n";
    commandResultMap[commandResult::SUCCESS_GAME_JOIN] = "Game successfully joined. Waiting for the owner to start\n";
    commandResultMap[commandResult::SUCCESS_GAME_START] = "Game Started!\n";
    commandResultMap[commandResult::SUCCESS_GAME_END] = "Game Ended. All players have been moved to server lobby.\n";
   commandResultMap[commandResult::SUCCESS_GAME_LEAVE] = "Game successfully left.\n";


    commandResultMap[commandResult::STRING_HELP] = 
         "\n\n                       Displaying Help Information\n"
         "  * To view the available games enter: games\n"
         "  * To create a new game from the options enter: create <game_index>\n"
         "  * To join a game with an invitation code enter: join <invitation_code>\n"
         "  * To exit the server enter: exit\n\n";
}