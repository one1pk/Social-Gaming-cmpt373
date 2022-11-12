#pragma once

#include "game.h"
#include "server.h"

#include <algorithm>
#include <sstream>
#include <unordered_map>
#include <vector>

/**
 * This interface represents the global state of the game server. It manages
 *  - all the clients connected to server as a whole and by game
 *  - various game instances
 *  - various games that are available to play
 */
class GlobalServerState {
public:
    GlobalServerState(unsigned update_interval) 
        : update_interval(update_interval) { 
        populateGameList(); 
    };

    // SERVER AND COMMAND SPECIFIC METHODS

    /**
     * Connects new users to the server for the very first time and adds them to  the respective lists.
     * NOTE: clears the connections vector after adding all users
     */
    void addNewConnections(std::vector<Connection>&);

    /**
     * This methods is called when users disconnect from the server
     * (voluntarily byt entering exit or involuntarily by force closing the terminal)
     */
    void disconnectConnection(Connection);

    /**
     * This method is called to help execution of JOIN command. It
     *   removes the client(user) from the server lobby and
     *   adds the client(user) to the game with invitationCode
     *
     */
    void addClientToGame(Connection, uintptr_t invitationCode);

    /**
     * This method is called to help execution of LEAVE command. It
     *  removes the client from the game they are currently playing and
     *  adds them to the server lobby.
     */
    void removeClientFromGame(Connection);

    /**
     * This methods helps execution of CREATE command. It
     *  creates a game object corresponding to the respective game configuration 
     *  with name == gameNameList[gameIndex] (found at ./gameconfigs/<name>.json)
     *  removes the client(user) from server lobby and makes them the owner of game instance
     *  adds them to the game lobby
     */
    uintptr_t createGame(int gameIndex, Connection connection);

    /**
     * Helps execution of START command.
     */
    void startGame(Connection connection);

    /**
     * Helps execution of END command.
     * Deletes the game instance where user is the owner. Move all players from game to server lobby
     */
    void endGame(Connection connection);

    /**
     * Processes all running games
     * - Sends out any player prompts to the respective players
     * - Sends out any game output to the main screen
     * - Checks whether player input has been received
     * - Ends games that are finished
     * Returns a deque of Messages to be sent out by the server
     */
    std::deque<Message> processGames();

    // GAME SPECIFIC METHODS

    std::string getGameNamesAsString();
    Connection getGameOwner(Connection connection);
    int getPlayerCount(Connection connection);
    bool isInGame(Connection connection);
    bool isGameIndex(int index);
    bool isOwner(Connection connection);
    bool gameHasEnoughPlayers(Connection connection);
    bool isOngoingGame(Connection connection);
    bool isOngoingGame(uintptr_t invitation_code);
    bool isValidGameInvitation(uintptr_t invitation_code);
    void registerUserGameInput(Connection connection, std::string input);

    // BROADCASTING METHODS

    /**
     * Builds messages for the server lobby with text as the passed in string.
     * Used to broadcast message to all the connected clients in server lobby
     */
    std::deque<Message> buildMessagesForServerLobby(std::string);

    /**
     * Builds messages for the game (that has user with passed in connection).
     * Used to broadcast passed in text to all other players.
     * NOTE: Doesn't send to owner or to current player <connection>
     */
    std::deque<Message> buildMsgsForOtherPlayers(std::string, Connection);

    /**
     * Builds messages for the game (that has user with passed in connection).
     * Used to broadcast passed in text to all the players.
     * NOTE: Doesn't send to owner
     */
    std::deque<Message> buildMsgsForAllPlayers(std::string, Connection);

    /**
     * Builds messages for the game (that has user with passed in connection).
     * Used to broadcast passed in text to all the players. and the game owner (main screen)
     */
    std::deque<Message> buildMsgsForAllPlayersAndOwner(std::string, Connection);

private:
    struct GameInput {
        std::string input;
        bool new_input;
        int time_remaining;
    };
    std::map<Connection, GameInput> user_game_input;

    unsigned update_interval;
    std::unordered_map<Connection, uintptr_t, ConnectionHash> clients_in_games;
    std::unordered_map<Connection, uintptr_t, ConnectionHash> gameOwnerMap;
    std::vector<Connection> clients;
    std::vector<Connection> clients_in_lobby;
    std::vector<Game> game_instances;

    std::unordered_map<int, std::string> gameNameList;

    void populateGameList();
    void removeGameInstance(uintptr_t gameID);

    void processGameMsgs(Game& game, std::deque<Message>& outgoing);
    void finishGame(Game& game, std::deque<Message>& outgoing);


    /**
     *  A general function that can be used on any vector of Connections to ease removing
     */
    void removeClientFromList(std::vector<Connection> &list, Connection connection);

    // GAME INSTANCE METHODS

    Game *getGameInstancebyUser(Connection connection);
    // Game *getGameInstancebyOwner(Connection connection);
    Game *getGameInstancebyInvitation(uintptr_t invitationCode); // TODO: FIX Invitation code in game
    Game *getGameInstancebyId(uintptr_t gameID);
};