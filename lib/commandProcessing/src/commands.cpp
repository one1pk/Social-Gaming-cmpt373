#include "commands.h"

commandResult Command::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    return executeImpl(connection, args, globalState, outgoing);
}

/////////////////////       SERVER COMMANDS       /////////////////////

//////////////////////////      CREATE GAME      //////////////////////////

commandResult CreateGameCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    if (globalState.isCurrentlyInGame(connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (args.empty()) {
        return commandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    int gameIndex;
    try {
        gameIndex = stoi(args[0]);
    } catch (std::exception &e) {
        return commandResult::ERROR_INVALID_GAME_INDEX;
    }

    if (!globalState.isGameIndex(gameIndex)) {
        return commandResult::ERROR_INVALID_GAME_INDEX;
    }
    return executeImpl(connection, args, globalState, outgoing);
}

commandResult CreateGameCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    int gameIndex = stoi(args[0]);

    uintptr_t invitationCode = globalState.createGame(gameIndex, connection);

    std::stringstream notification;
    notification << "Game Successfully Created! \nInvitationCode = " << invitationCode << "\n";

    outgoing.push_back({connection, notification.str()});

    return commandResult::SUCCESS;
}

//////////////////////////      LIST GAMES      //////////////////////////

commandResult ListGamesCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    if (globalState.isCurrentlyInGame(connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }
    outgoing.push_back({connection, globalState.getGameNamesAsString()});
    return commandResult::SUCCESS;
}

commandResult ListHelpCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    return commandResult::STRING_HELP;
}


//////////////////////////      JOIN GAME      //////////////////////////

commandResult JoinGameCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    if (globalState.isCurrentlyInGame(connection) || globalState.isOwner(connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (args.empty()) {
        return commandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    int invitationCode = stoi(args[0]);

    if (!globalState.isValidGameInvitation(invitationCode)) {
        return commandResult::ERROR_INVALID_INVITATION_CODE;
    }

    return executeImpl(connection, args, globalState, outgoing);
}

commandResult JoinGameCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    int invitationCode = stoi(args[0]);
    globalState.addClientToGame(connection, invitationCode);
    int playerCount = globalState.getPlayerCount(connection);

    // FIX Strings
    std::stringstream notification;
    notification << "New user joined the game.\nPlayer Count = " << playerCount << "\n";

    std::deque<Message> messages = globalState.buildMessagesForGame(notification.str(), connection);
    for(auto message : messages){
        outgoing.push_back(message);
    }

    return commandResult::SUCCESS_GAME_JOIN;
}

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

//////////////////////////      START GAME      //////////////////////////

commandResult StartGameCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    if(!globalState.isCurrentlyInGame(connection)){
        return commandResult::ERROR_INVALID_COMMAND;
    }
    
    if (!globalState.isOwner(connection)) {
        return commandResult::ERROR_NOT_AN_OWNER;
    }

    if (globalState.isOngoingGame(connection)) {
        return commandResult::ERROR_INVALID_START_GAME_COMMAND;
    }

    return executeImpl(connection, args, globalState, outgoing);
}

commandResult StartGameCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    globalState.startGame(connection);

    std::deque<Message> messages = globalState.buildMessagesForGame("Game Started!\n", connection);
    for(auto message : messages){
        outgoing.push_back(message);
    }

    return commandResult::SUCCESS_GAME_START;
}

//////////////////////////      END GAME      //////////////////////////

commandResult EndGameCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    if(!globalState.isCurrentlyInGame(connection)){
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (!globalState.isOwner(connection)) {
        return commandResult::ERROR_NOT_AN_OWNER;
    }
    return executeImpl(connection, args, globalState, outgoing);
}

commandResult EndGameCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    std::deque<Message> messages = globalState.buildMessagesForGame("Game Ended!\n", connection);
    for(auto message : messages){
        outgoing.push_back(message);
    }

    globalState.endGame(connection);

    return commandResult::SUCCESS_GAME_END;
}

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

//////////////////////////      LEAVE GAME      //////////////////////////

commandResult LeaveGameCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    if(!globalState.isCurrentlyInGame(connection)){
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (globalState.isOwner(connection)) {
        return commandResult::ERROR_OWNER_CANNOT_LEAVE;
    }
    return executeImpl(connection, args, globalState, outgoing);
}

commandResult LeaveGameCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    
    int playerCount = globalState.getPlayerCount(connection);

    std::stringstream notification;
    notification << "A user left the game.\nPlayer Count = " << playerCount << "\n";
    std::deque<Message> messages = globalState.buildMessagesForGame(notification.str() , connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.removeClientFromGame(connection);

    return commandResult::SUCCESS_GAME_LEAVE;
}

//////////////////////////      GENERAL TEXT      //////////////////////////

commandResult TextCommand::execute(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {
    if (globalState.isCurrentlyInGame(connection)) {
        return executeInGameImpl(connection, args, globalState, outgoing);
    } else {
        return executeImpl(connection, args, globalState, outgoing);
    }
}

commandResult TextCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    std::stringstream outgoingText;
    outgoingText << connection.id << " : " << args [0] << "\n";

    std::deque<Message> messages = globalState.buildMessagesForLobby(outgoingText.str());
    for (auto message : messages) {
        outgoing.push_back(message);
    }
    
    return commandResult::SUCCESS;
}

commandResult TextCommand::executeInGameImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

    std::stringstream outgoingText;
    outgoingText << connection.id << " : " << args [0] << "\n";

    std::deque<Message> messages = globalState.buildMessagesForGame(outgoingText.str() , connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }
    return commandResult::SUCCESS;
}


// //////////////////////////      EXIT SERVER      //////////////////////////

// // TODO: Redundant as it never reaches the end user.
// commandResult ExitServerCommand::executeImpl(Connection connection, std::vector<std::string> args, GlobalServerState &globalState, std::deque<Message>& outgoing) {

//     return commandResult::SUCCESS;
// }