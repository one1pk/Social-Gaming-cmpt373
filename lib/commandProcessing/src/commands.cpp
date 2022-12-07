#include "commands.h"

////////////////////////////////////////////////////////////////////////////
////////////////////////      HELPER FUNCTIONS      ////////////////////////

void handlePlayerLeave(GlobalServerState& globalState, std::deque<Message>& outgoing, User user) {
    std::stringstream notification;
    notification << "\n" << globalState.getName(user) << " left\n\n" ; 
    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(notification.str(), user);
    for (auto message : messages) {
        outgoing.push_back(message);
    }
    
    User owner = globalState.getGameOwner(user);
    int playerCount = globalState.getPlayerCount(user);

    globalState.removeClientFromGame(user);

    

    if (globalState.isOngoingGame(owner) && !globalState.gameHasEnoughPlayers(owner)) {
        std::deque<Message> messages = globalState.buildMsgsForAllPlayersAndOwner("\nNot enough players left, ending game.\n\n", owner);
        for (auto message : messages) {
            outgoing.push_back(message);
        }
        globalState.endGame(owner);
    } else {
        notification << "Player Count : " << playerCount - 1 << "\n\n";
        outgoing.push_back({owner, notification.str()});
    }
}

////////////////////////////////////////////////////////////////////////////
/////////////////////////      SERVER COMMAND      /////////////////////////

//////////////////////////      CREATE GAME      ///////////////////////////

CommandResult CreateGameCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.user)) {
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

    uintptr_t invitationCode = globalState.createGame(gameIndex, processedMessage.user);

    std::stringstream notification;
    notification << "Game Successfully Created! \nInvitationCode = " << invitationCode << "\n\n";
    outgoing.push_back({processedMessage.user, notification.str()});

    return CommandResult::SUCCESS;
}

//////////////////////////      LIST GAMES      //////////////////////////
CommandResult ListGamesCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    outgoing.push_back({processedMessage.user, globalState.getGameNamesAsString()});
    return CommandResult::SUCCESS;
}

//////////////////////////      HELP      //////////////////////////

CommandResult ListHelpCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.user)) {
        return CommandResult::STRING_INGAME_OWNER_HELP;
    }
    if (globalState.isInGame(processedMessage.user)) {
        return CommandResult::STRING_INGAME_PLAYER_HELP;
    }
    return CommandResult::STRING_SERVER_HELP;
}

//////////////////////////      JOIN GAME      //////////////////////////

CommandResult JoinGameCommand::execute(ProcessedMessage &processedMessage) {
    if (processedMessage.arguments.empty()) {
        return CommandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }
    if (globalState.isOwner(processedMessage.user)) {
        return CommandResult::ERROR_OWNER_CANNOT_JOIN_FROM_SAME_DEVICE;
    }
    if (globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }

    int invitationCode;
    try {
        invitationCode = stoi(processedMessage.arguments[0]);
    } catch (std::exception &e) {
        return CommandResult::ERROR_INVALID_INVITATION_CODE;
    }
    
    if (!globalState.isValidGameInvitation(invitationCode)) {
        return CommandResult::ERROR_INVALID_INVITATION_CODE;
    }
    if (globalState.isOngoingGame(invitationCode)) {
        return CommandResult::ERROR_GAME_HAS_STARTED;
    }

    globalState.addClientToGame(processedMessage.user, invitationCode);
    int playerCount = globalState.getPlayerCount(processedMessage.user);

    std::stringstream notification;
    notification << "\n" << globalState.getName(processedMessage.user) << " joined the game!\n\n";

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(notification.str(), processedMessage.user);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    notification << "Player Count : " << playerCount << "\n\n";
    outgoing.push_back({globalState.getGameOwner(processedMessage.user), notification.str()});

    return CommandResult::SUCCESS_GAME_JOIN;
}

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

//////////////////////////      START GAME      //////////////////////////

CommandResult StartGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (!globalState.isOwner(processedMessage.user)) {
        return CommandResult::ERROR_NOT_AN_OWNER;
    }
    if (globalState.isOngoingGame(processedMessage.user)) {
        return CommandResult::ERROR_GAME_HAS_STARTED;
    }
    if (!globalState.gameHasEnoughPlayers(processedMessage.user)) {
        return CommandResult::ERROR_NOT_ENOUGH_PLAYERS;
    }

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers("\nGame Started!\n\n", processedMessage.user);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.startGame(processedMessage.user);

    return CommandResult::SUCCESS_GAME_START;
}

//////////////////////////      END GAME      //////////////////////////

CommandResult EndGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (!globalState.isOwner(processedMessage.user)) {
        return CommandResult::ERROR_NOT_AN_OWNER;
    }

    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers("\nGame Ended!\n\n", processedMessage.user);
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.user);

    return CommandResult::SUCCESS_GAME_END;
}

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

//////////////////////////      LEAVE GAME      //////////////////////////

CommandResult LeaveGameCommand::execute(ProcessedMessage &processedMessage) {
    if (!globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }
    if (globalState.isOwner(processedMessage.user)) {
        return CommandResult::ERROR_OWNER_CANNOT_LEAVE;
    }

    handlePlayerLeave(globalState, outgoing, processedMessage.user);

    return CommandResult::SUCCESS_GAME_LEAVE;
}

//////////////////////////      EXIT SERVER      //////////////////////////

CommandResult ExitServerCommand::execute(ProcessedMessage &processedMessage) {
    if (globalState.isOwner(processedMessage.user)) {
        return executeOwnerImpl(processedMessage);
    }
    if (globalState.isInGame(processedMessage.user)) {
        return executePlayerImpl(processedMessage);
    }

    return CommandResult::SUCCESS;
}

CommandResult ExitServerCommand::executePlayerImpl(ProcessedMessage &processedMessage) {
    handlePlayerLeave(globalState, outgoing, processedMessage.user);
    globalState.disconnectUser(processedMessage.user);

    return CommandResult::SUCCESS;
}

CommandResult ExitServerCommand::executeOwnerImpl(ProcessedMessage &processedMessage) {
    std::deque<Message> messages = globalState.buildMsgsForOtherPlayers(
        "\nOwner left the server. Game Ended!\n\n",
        processedMessage.user
    );
    for (auto message : messages) {
        outgoing.push_back(message);
    }

    globalState.endGame(processedMessage.user);
    globalState.disconnectUser(processedMessage.user);

    return CommandResult::SUCCESS;
}

//////////////////////////      SET OR CHANGE USERNAME      //////////////////////////

CommandResult UserNameCommand::execute(ProcessedMessage &processedMessage) {
    if (processedMessage.arguments.empty()) {
        return CommandResult::ERROR_INCORRECT_COMMAND_FORMAT;
    }

    if (globalState.isInGame(processedMessage.user)) {
        return CommandResult::ERROR_INVALID_COMMAND;
    }

    if (!globalState.isInLobby(processedMessage.user)) {
        globalState.addClientToLobby(processedMessage.user);
    }
    globalState.setName(processedMessage.user, processedMessage.arguments[0]);

    std::stringstream notification;
    notification << "\n" << "Hello, "<<processedMessage.arguments[0] << "!\n";
    notification << "You are now in the main lobby, here you can chat with other players in the lobby\n";

    outgoing.push_back({processedMessage.user, notification.str()});
    
    return CommandResult::STRING_SERVER_HELP;
}
