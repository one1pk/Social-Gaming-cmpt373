#pragma once

#include "globalState.h"

#include <algorithm>
#include <string>
#include <unordered_map>
#include <vector>

enum class commandResult {
    ERROR_INCORRECT_COMMAND_FORMAT,
    ERROR_INVALID_GAME_INDEX,
    ERROR_INVALID_INVITATION_CODE,
    ERROR_INVALID_START_GAME_COMMAND,
    ERROR_NOT_AN_OWNER,
    ERROR_OWNER_CANNOT_LEAVE,
    ERROR_INVALID_COMMAND,
    ERROR,

    SUCCESS,
    SUCCESS_GAME_CREATION,
    SUCCESS_GAME_JOIN,
    SUCCESS_GAME_START,
    SUCCESS_GAME_END,
    SUCCESS_GAME_LEAVE,

    STRING_HELP,

};


class Command {
public:
    Command() = default;
    ~Command() = default;
    virtual commandResult execute(Connection c, std::vector<std::string> args, GlobalServerState&, std::deque<Message>& outgoing);

private:
    virtual commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) = 0;
};

/////////////////////       SERVER COMMANDS       /////////////////////

class CreateGameCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

class ListGamesCommand : public Command {
private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

class ListHelpCommand : public Command {
private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

class JoinGameCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

/////////////////////       IN GAME COMMANDS - OWNER      /////////////////////

class StartGameCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

class EndGameCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

/////////////////////       IN GAME COMMANDS - PLAYER      /////////////////////

class LeaveGameCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};

class TextCommand : public Command {
public:
    commandResult execute(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&) override;

private:
    commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
    commandResult executeInGameImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
};


//////////////////////////////          EXTRAS          /////////////////////////////


// class ExitServerCommand : public Command {
// private:
//     commandResult executeImpl(Connection c, std::vector<std::string> args,GlobalServerState&, std::deque<Message>&);
// };