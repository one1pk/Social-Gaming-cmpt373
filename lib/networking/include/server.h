#pragma once

#include <deque>
#include <string>
#include <memory>

/**
 *  An identifier for a Client connected to a Server. The ID of a User is
 *  guaranteed to be unique across all actively connected Client instances.
 */
struct User {
    uintptr_t id;

    bool operator==(User other) const {
        return id == other.id;
    }
    bool operator<(User other) const {
        return id < other.id;
    }
};

struct UserHash {
  size_t
  operator()(User c) const {
    return std::hash<decltype(c.id)>{}(c.id);
  }
};


/**
 *  A Message containing text that can be sent to or was recieved from a given
 *  User.
 */
struct Message {
    User user;
    std::string text;
};


/** A compilation firewall for the server. */
class ServerImpl;

struct ServerImplDeleter {
    void operator()(ServerImpl* serverImpl);
};

/**
 *  @class Server
 *
 *  @brief A single threaded network server for transferring text.
 *
 *  The Server class transfers text to and from multiple Client instances
 *  connected on a given port. The behavior is single threaded, so all transfer
 *  operations are grouped and performed on the next call to Server::update().
 *  Text can be sent to the Server using Client::send() and received from the
 *  Server using Client::receive().
 *
 *  The Server is websocket based and supports sending a single file back in
 *  response to HTTP requests for `index.html`. This allows command line and
 *  web clients to interact.
 */
class Server {
public:
  /**
   *  Construct a Server that listens for connections on the given port.
   *  The onConnect and onDisconnect arguments are callbacks called when a
   *  Client connects or disconnects from the Server respectively.
   *
   *  The callbacks can be functions, function pointers, lambdas, or any other
   *  callable construct. They should support the signature:
   *      void onConnect(User c);
   *      void onDisconnect(User c);
   *  The User class is an identifier for each connected Client. It
   *  contains an ID that is guaranteed to be unique across all active
   *  connections.
   *
   *  The httpMessage is a string containing HTML content that will be sent
   *  in response to standard HTTP requests for any path ending in `index.html`.
   */
    template <typename C, typename D>
    Server(unsigned short port, std::string httpMessage, C onConnect, D onDisconnect)
        : connectionHandler{std::make_unique<ConnectionHandlerImpl<C,D>>(onConnect, onDisconnect)},
        impl{buildImpl(*this, port, std::move(httpMessage))} 
    { }

    /**
     *  Perform all pending sends and receives. This function can throw an
     *  exception if any of the I/O operations encounters an error.
     */
    void update();

    /**
     *  Send a list of messages to their respective Clients.
     */
    void send(const std::deque<Message>& messages);

    /**
     *  Receive Messages from Clients. Returns all Messages collected 
     * by previous calls to Server::update() and not yet received.
     */
    [[nodiscard]] std::deque<Message> receive();

    /**
     *  Disconnect the Client specified by the given User.
     */
    void disconnect(User user);

private:
    friend class ServerImpl;

    // Hiding the template parameters of the Server class behind a pointer to
    // a private interface allows us to refer to an unparameterized Server
    // object while still having the handlers of connect & disconnect be client
    // defined types. This is a form of *type erasure*.
    class ConnectionHandler {
    public:
        virtual ~ConnectionHandler() = default;
        virtual void handleConnect(User) = 0;
        virtual void handleDisconnect(User) = 0;
    };

    template <typename C, typename D>
    class ConnectionHandlerImpl final : public ConnectionHandler {
    public:
        ConnectionHandlerImpl(C onConnect, D onDisconnect)
            : onConnect{std::move(onConnect)},
            onDisconnect{std::move(onDisconnect)}
            { }
        ~ConnectionHandlerImpl() override = default;
        void handleConnect(User c)    override { onConnect(c);    }
        void handleDisconnect(User c) override { onDisconnect(c); }
    private:
        C onConnect;
        D onDisconnect;
    };

    static std::unique_ptr<ServerImpl,ServerImplDeleter>
    buildImpl(Server& server, unsigned short port, std::string httpMessage);

    std::unique_ptr<ConnectionHandler> connectionHandler;
    std::unique_ptr<ServerImpl,ServerImplDeleter> impl;
};

