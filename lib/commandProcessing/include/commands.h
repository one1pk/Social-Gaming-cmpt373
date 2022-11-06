#pragma once

#include "globalState.h"
#include "messageProcessor.h"

#include <algorithm>
#include <string>
#include <unordered_map>

/**
 * Possible results that different commands can produce. Returned in the execution of each command
 */
enum class commandResult {
    ERROR_INCORRECT_COMMAND_FORMAT,
    ERROR_INVALID_GAME_INDEX,
    ERROR_INVALID_INVITATION_CODE,
    ERROR_INVALID_START_GAME_COMMAND,
    ERROR_GAME_NOT_STARTED,
    ERROR_NOT_AN_OWNER,
    ERROR_OWNER_CANNOT_LEAVE,
    ERROR_OWNER_CANNOT_JOIN_FROM_SAME_DEVICE,
    ERROR_INVALID_COMMAND,
    ERROR,

    SUCCESS,
    SUCCESS_GAME_CREATION,
    SUCCESS_GAME_JOIN,
    SUCCESS_GAME_START,
    SUCCESS_GAME_END,
    SUCCESS_GAME_LEAVE,

    STRING_SERVER_HELP,
    STRING_INGAME_PLAYER_HELP,
    STRING_INGAME_OWNER_HELP,

};

/**
 * A simple executable command interface
 */
class Command {
public:
    Command() = default;
    ~Command() = default;
    virtual commandResult execute(ProcessedMessage &);

private:
    virtual commandResult executeImpl(ProcessedMessage &) = 0;
};

/////////////////////       SERVER COMMANDS       /////////////////////

/**
 * NOTE:
 * Each derived command contains
 *
 *  - the globalState reference and utilises its methods to make changes
 *    to the global state when executing commands
 *
 *  - the outgoing reference field that is a reference to command handler's outgoing message queue.
 *    It enables each command to push multiple messages inside the execution function instead of
 *    returning a queue.
 *
 */

/**
 * Creates an instance of the game represented by command argument
 * Assigns user as the owner of that game instance
 * Returns Invitation code as feedback to the user
 *
 * Returns error if:
 *      No arguments provided
 *      The user is already in a game
 *      The argument is invalid or not an integer
 *      The game index doesn't exist
 */
class CreateGameCommand : public Command {
public:
    CreateGameCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/**
 * Lists all the games that are available on the server
 */
class ListGamesCommand : public Command {
public:
    ListGamesCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &);

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/**
 * Lists all the commands that a user can enter based on where they are:
 * In lobby, In game (owner), In game (player)
 */
class ListHelpCommand : public Command {
public:
    ListHelpCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &);

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
    commandResult executeIngameOwnerImpl(ProcessedMessage &);
    commandResult executeIngamePlayerImpl(ProcessedMessage &);
};

/**
 * Allows the user to join the game by specifying an invitation code.
 *
 * Returns error if:
 *      - No arguments provided
 *      - User is an owner (owner cannot join game from same window)
 *      - User is currently in game
 *      - Invalid invitation code
 */
class JoinGameCommand : public Command {
public:
    JoinGameCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

/**
 * Starts the game and notifies everyone involved.
 *
 * Returns error if:
 *      - User is in lobby
 *      - User is not an owner
 *      - Game is already ongoing
 */
class StartGameCommand : public Command {
public:
    StartGameCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/**
 * Ends the game and notifies all the players
 *
 * Returns error if:
 *      - User is in lobby
 *      - User is not an owner
 */
class EndGameCommand : public Command {
public:
    EndGameCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

/**
 * Removes user from the game they are currently joined in.
 *
 * Returns an error if:
 *      - User is not in any game
 *      - User is an owner
 */
class LeaveGameCommand : public Command {
public:
    LeaveGameCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
};

/**
 * Broadcasts messages to other users based on where the sender is: in lobby or in game
 * Doesn't broadcast to the owner (if an in game message) however broadcasts each of owner's message
 * to the players in game lobby!
 *
 * Any messages not mapped to commands are by default treated as chat commands
 */
class ChatCommand : public Command {
public:
    ChatCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
    commandResult executeInGameImpl(ProcessedMessage &);
};

////////////////////////////          EXTRAS          /////////////////////////////

/**
 * Handles the post processing of a user exit basesd on their role.
 * If a lobby user:
 *      do nothing
 * If a game player:
 *      - disconnect from global state
 *      - notify everyone else
 * If a game owner:
 *      - end the game and move other players to lobby
 *      - disconnect from global state
 *      - notify everyone else
 */
class ExitServerCommand : public Command {
public:
    ExitServerCommand(GlobalServerState &globalState, std::deque<Message> &outgoing)
        : globalState{globalState},
          outgoing{outgoing} {}
    commandResult execute(ProcessedMessage &) override;

private:
    GlobalServerState &globalState;
    std::deque<Message> &outgoing;
    commandResult executeImpl(ProcessedMessage &);
    commandResult executePlayerImpl(ProcessedMessage &);
    commandResult executeOwnerImpl(ProcessedMessage &);
};