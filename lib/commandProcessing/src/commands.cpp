#include "commands.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////      HELPER FUNCTIONS      ////////////////////////

void handlePlayerLeave(GlobalServerState& globalState, std::deque<Message>& outgoing, Connection connection) {
    std::stringstream notification;
    notification << "\nUser " << connection.id << " left\n\n" ; 
    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(notification.str(), connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }
    
    Connection owner = globalState.getGameOwner(connection);
    globalState.removeClientFromGame(connection);

    if (globalState.isOngoingGame(owner) && !globalState.gameHasEnoughPlayers(owner)) {
        std::deque<Message> messages = globalState.buildMsgsForAllPlayersAndOwner("\nNot enough players left, ending game.\n\n", owner);
        for (auto message : messages) {
            outgoing.push_back(message);
        }
        globalState.endGame(owner);
    }
}

////////////////////////////////////////////////////////////////////////////
/////////////////////////      SERVER COMMAND      /////////////////////////

//////////////////////////      CREATE GAME      ///////////////////////////

CommandResult CreateGameCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (processedMessage.arguments.empty()) {
        return CommandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    int gameIndex;
    try {
        gameIndex = stoi(processedMessage.arguments[0]);
    } catch (std::exception &e) {
        return CommandResult::ERROR_INVALID_GAME_INDEX;
    }

    if (!globalState.isGameIndex(gameIndex)) {
        return CommandResult::ERROR_INVALID_GAME_INDEX;
    }

    uintptr_t invitationCode = globalState.createGame(gameIndex, processedMessage.connection);

    std::stringstream notification;
    notification << "Game Successfully Created! \nInvitationCode = " << invitationCode << "\n\n";
    outgoing.push_back({processedMessage.connection, notification.str()});

    return CommandResult::SUCCESS;
}

//////////////////////////      LIST GAMES      //////////////////////////
CommandResult ListGamesCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    outgoing.push_back({processedMessage.connection, globalState.getGameNamesAsString()});
    return CommandResult::SUCCESS;
}

//////////////////////////      HELP      //////////////////////////

CommandResult ListHelpCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.connection)) {
        return CommandResult::STRING_INGAME_OWNER_HELP;
    }
    if (globalState.isInGame(processedMessage.connection)) {
        return CommandResult::STRING_INGAME_PLAYER_HELP;
    }
    return CommandResult::STRING_SERVER_HELP;
}

//////////////////////////      JOIN GAME      //////////////////////////

CommandResult JoinGameCommand::execute(ProcessedMessage &processedMessage) {
    if (processedMessage.arguments.empty()) {
        return CommandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }
    if (globalState.isOwner(processedMessage.connection)) {
        return CommandResult::ERROR_OWNER_CANNOT_JOIN_FROM_SAME_DEVICE;
    }
    if (globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }

    int invitationCode = stoi(processedMessage.arguments[0]);
    if (!globalState.isValidGameInvitation(invitationCode)) {
        return CommandResult::ERROR_INVALID_INVITATION_CODE;
    }
    if (globalState.isOngoingGame(invitationCode)) {
        return CommandResult::ERROR_GAME_HAS_STARTED;
    }

    globalState.addClientToGame(processedMessage.connection, invitationCode);
    int playerCount = globalState.getPlayerCount(processedMessage.connection);

    std::stringstream notification;
    notification << "\nUser " << processedMessage.connection.id << " joined the game!\n\n";

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(notification.str(), processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    notification << "Player Count : " << playerCount << "\n\n";
    // FIX: Establish owner as connection in game rather than uintptr_t
    // outgoing.push_back({globalState.getGameOwnerConnection(processedMessage.connection), notification.str()});

    return CommandResult::SUCCESS_GAME_JOIN;
}

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

//////////////////////////      START GAME      //////////////////////////

CommandResult StartGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (!globalState.isOwner(processedMessage.connection)) {
        return CommandResult::ERROR_NOT_AN_OWNER;
    }
    if (globalState.isOngoingGame(processedMessage.connection)) {
        return CommandResult::ERROR_GAME_HAS_STARTED;
    }
    if (!globalState.gameHasEnoughPlayers(processedMessage.connection)) {
        return CommandResult::ERROR_NOT_ENOUGH_PLAYERS;
    }

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers("\nGame Started!\n\n", processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.startGame(processedMessage.connection);

    return CommandResult::SUCCESS_GAME_START;
}

//////////////////////////      END GAME      //////////////////////////

CommandResult EndGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (!globalState.isOwner(processedMessage.connection)) {
        return CommandResult::ERROR_NOT_AN_OWNER;
    }

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers("\nGame Ended!\n\n", processedMessage.connection);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.connection);

    return CommandResult::SUCCESS_GAME_END;
}

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

//////////////////////////      LEAVE GAME      //////////////////////////

CommandResult LeaveGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.connection)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (globalState.isOwner(processedMessage.connection)) {
        return CommandResult::ERROR_OWNER_CANNOT_LEAVE;
    }

    handlePlayerLeave(globalState, outgoing, processedMessage.connection);

    return CommandResult::SUCCESS_GAME_LEAVE;
}

//////////////////////////      EXIT SERVER      //////////////////////////

CommandResult ExitServerCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.connection)) {
        return executeOwnerImpl(processedMessage);
    }
    if (globalState.isInGame(processedMessage.connection)) {
        return executePlayerImpl(processedMessage);
    }

    return CommandResult::SUCCESS;
}

// FIX: DRY Violation
CommandResult ExitServerCommand::executePlayerImpl(ProcessedMessage &processedMessage) {
    handlePlayerLeave(globalState, outgoing, processedMessage.connection);
    globalState.disconnectConnection(processedMessage.connection);

    return CommandResult::SUCCESS;
}

// FIX: DRY Violation
CommandResult ExitServerCommand::executeOwnerImpl(ProcessedMessage &processedMessage) {
    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(
        "\nOwner left the server. Game Ended!\n\n",
        processedMessage.connection
    );
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.connection);
    globalState.disconnectConnection(processedMessage.connection);

    return CommandResult::SUCCESS;
}