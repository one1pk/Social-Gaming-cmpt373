#include "commands.h"

commandResult Command::execute(ProcessedMessage &processedMessage) {
    return executeImpl(processedMessage);
}

/////////////////////       SERVER COMMANDS       /////////////////////

//////////////////////////      CREATE GAME      //////////////////////////

commandResult CreateGameCommand::execute(ProcessedMessage &processedMessage) {

    if (globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (processedMessage.arguments.empty()) {
        return commandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    int gameIndex;
    try {
        gameIndex = stoi(processedMessage.arguments[0]);
    } catch (std::exception &e) {
        return commandResult::ERROR_INVALID_GAME_INDEX;
    }

    if (!globalState.isGameIndex(gameIndex)) {
        return commandResult::ERROR_INVALID_GAME_INDEX;
    }
    return executeImpl(processedMessage);
}

commandResult CreateGameCommand::executeImpl(ProcessedMessage &processedMessage) {

    int gameIndex = stoi(processedMessage.arguments[0]);

    uintptr_t invitationCode = globalState.createGame(gameIndex, processedMessage.connection);

    std::stringstream notification;
    notification << "Game Successfully Created! \nInvitationCode = " << invitationCode << "\n\n";

    outgoing.push_back({processedMessage.connection, notification.str()});

    return commandResult::SUCCESS;
}

//////////////////////////      LIST GAMES      //////////////////////////
commandResult ListGamesCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }
    return executeImpl(processedMessage);
}

commandResult ListGamesCommand::executeImpl(ProcessedMessage &processedMessage) {
    outgoing.push_back({processedMessage.connection, globalState.getGameNamesAsString()});
    return commandResult::SUCCESS;
}

commandResult ListHelpCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.connection)) {
        return executeIngameOwnerImpl(processedMessage);
    }

    if (globalState.isInGame(processedMessage.connection)) {
        return executeIngamePlayerImpl(processedMessage);
    }

    return executeImpl(processedMessage);
}
commandResult ListHelpCommand::executeIngameOwnerImpl(ProcessedMessage &processedMessage) {
    return commandResult::STRING_INGAME_OWNER_HELP;
}
commandResult ListHelpCommand::executeIngamePlayerImpl(ProcessedMessage &processedMessage) {
    return commandResult::STRING_INGAME_PLAYER_HELP;
}
commandResult ListHelpCommand::executeImpl(ProcessedMessage &processedMessage) {
    return commandResult::STRING_SERVER_HELP;
}

//////////////////////////      JOIN GAME      //////////////////////////

commandResult JoinGameCommand::execute(ProcessedMessage &processedMessage) {
    if (processedMessage.arguments.empty()) {
        return commandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    if (globalState.isOwner(processedMessage.connection)) {
        return commandResult::ERROR_OWNER_CANNOT_JOIN_FROM_SAME_DEVICE;
    }

    if (globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    int invitationCode = stoi(processedMessage.arguments[0]);

    if (!globalState.isValidGameInvitation(invitationCode)) {
        return commandResult::ERROR_INVALID_INVITATION_CODE;
    }

    if (globalState.isOngoingGame(invitationCode)) {
        return commandResult::ERROR_GAME_HAS_STARTED;
    }

    return executeImpl(processedMessage);
}

commandResult JoinGameCommand::executeImpl(ProcessedMessage &processedMessage) {

    int invitationCode = stoi(processedMessage.arguments[0]);

    globalState.addClientToGame(processedMessage.connection, invitationCode);
    int playerCount = globalState.getPlayerCount(processedMessage.connection);

    std::stringstream notification;
    notification << "\nUser " << processedMessage.connection.id << " joined the game!\n\n";

    std::deque<Message> messages = globalState.buildMessagesForGame(notification.str(), processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    notification << "Player Count : " << playerCount << "\n\n";
    // FIX: Establish owner as connection in game rather than uintptr_t
    // outgoing.push_back({globalState.getGameOwnerConnection(processedMessage.connection), notification.str()});

    return commandResult::SUCCESS_GAME_JOIN;
}

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

//////////////////////////      START GAME      //////////////////////////

commandResult StartGameCommand::execute(ProcessedMessage &processedMessage) {

    if (!globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (!globalState.isOwner(processedMessage.connection)) {
        return commandResult::ERROR_NOT_AN_OWNER;
    }

    if (globalState.isOngoingGame(processedMessage.connection)) {
        return commandResult::ERROR_GAME_HAS_STARTED;
    }

    if (!globalState.isGameReady(processedMessage.connection)) {
        return commandResult::ERROR_NOT_ENOUGH_PLAYERS;
    }

    return executeImpl(processedMessage);
}

commandResult StartGameCommand::executeImpl(ProcessedMessage &processedMessage) {
    std::deque<Message> messages = globalState.buildMessagesForGame("\nGame Started!\n\n", processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.startGame(processedMessage.connection);

    return commandResult::SUCCESS_GAME_START;
}

//////////////////////////      END GAME      //////////////////////////

commandResult EndGameCommand::execute(ProcessedMessage &processedMessage) {

    if (!globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (!globalState.isOwner(processedMessage.connection)) {
        return commandResult::ERROR_NOT_AN_OWNER;
    }

    if (!globalState.isOngoingGame(processedMessage.connection)) {
        return commandResult::ERROR_GAME_NOT_STARTED;
    }

    return executeImpl(processedMessage);
}

commandResult EndGameCommand::executeImpl(ProcessedMessage &processedMessage) {

    std::deque<Message> messages = globalState.buildMessagesForGame("\nGame Ended!\n\n", processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.connection);

    return commandResult::SUCCESS_GAME_END;
}

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

//////////////////////////      LEAVE GAME      //////////////////////////

commandResult LeaveGameCommand::execute(ProcessedMessage &processedMessage) {

    if (!globalState.isInGame(processedMessage.connection)) {
        return commandResult::ERROR_INVALID_COMMAND;
    }

    if (globalState.isOwner(processedMessage.connection)) {
        return commandResult::ERROR_OWNER_CANNOT_LEAVE;
    }
    return executeImpl(processedMessage);
}

commandResult LeaveGameCommand::executeImpl(ProcessedMessage &processedMessage) {

    int playerCount = globalState.getPlayerCount(processedMessage.connection) - 1;

    std::stringstream notification;
    notification << "\nUser " << processedMessage.connection.id << " left the game.\n\n";
    std::deque<Message> messages = globalState.buildMessagesForGame(notification.str(), processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    notification <<  "Player Count = " << playerCount << "\n\n";
    // FIX: Establish owner as connection in game rather than uintptr_t
    // outgoing.push_back({globalState.getGameOwnerConnection(processedMessage.connection), notification.str()});

    globalState.removeClientFromGame(processedMessage.connection);

    return commandResult::SUCCESS_GAME_LEAVE;
}

//////////////////////////      EXIT SERVER      //////////////////////////

commandResult ExitServerCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.connection)) {
        return executeOwnerImpl(processedMessage);
    }

    if (globalState.isInGame(processedMessage.connection)) {
        return executePlayerImpl(processedMessage);
    }

    return executeImpl(processedMessage);
}

commandResult ExitServerCommand::executeImpl(ProcessedMessage &processedMessage) {

    return commandResult::SUCCESS;
}

// FIX: DRY Violation
commandResult ExitServerCommand::executePlayerImpl(ProcessedMessage &processedMessage) {
    int playerCount = globalState.getPlayerCount(processedMessage.connection) - 1;

    std::stringstream notification;
    notification << "\nUser " << processedMessage.connection.id << " left the server\n\n" ;
    std::deque<Message> messages = globalState.buildMessagesForGame(notification.str(), processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    notification << "Player Count = " << playerCount << "\n\n";

    // outgoing.push_back({globalState.getGameOwnerConnection(processedMessage.connection), notification.str()});

    globalState.removeClientFromGame(processedMessage.connection);
    globalState.disconnectConnection(processedMessage.connection);

    return commandResult::SUCCESS;
}

// FIX: DRY Violation
commandResult ExitServerCommand::executeOwnerImpl(ProcessedMessage &processedMessage) {

    std::deque<Message> messages = globalState.buildMessagesForGame("\nOwner left the server. Game Ended!\n\n", processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.connection);
    globalState.disconnectConnection(processedMessage.connection);

    return commandResult::SUCCESS;
}


/*/////////////////////////      GENERAL TEXT      //////////////////////////

// Can stop the in game chat by simply returning something else from the if statement
commandResult ChatCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.connection)) {
        return executeInGameImpl(processedMessage);
    }
    return executeImpl(processedMessage);
}

commandResult ChatCommand::executeImpl(ProcessedMessage &processedMessage) {

    std::stringstream outgoingText;
    outgoingText << processedMessage.connection.id << " : " << processedMessage.arguments[0] << "\n";

    std::deque<Message> messages = globalState.buildMessagesForServerLobby(outgoingText.str());
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    return commandResult::SUCCESS;
}

commandResult ChatCommand::executeInGameImpl(ProcessedMessage &processedMessage) {

    std::stringstream outgoingText;
    outgoingText << processedMessage.connection.id << " : " << processedMessage.arguments[0] << "\n";

    std::deque<Message> messages = globalState.buildMessagesForGame(outgoingText.str(), processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }
    outgoing.push_back({processedMessage.connection, outgoingText.str()});

    return commandResult::SUCCESS;
}
*/
