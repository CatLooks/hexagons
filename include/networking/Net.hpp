#pragma once
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <unordered_set>
#include "templated/delegate.hpp" 
#include "networking/EOSManager.hpp"
#include "networking/threadsafe_queue.hpp"

/// Packet Types

/// Network packet containing sender id and raw data.
struct NetPacket {
    std::string senderId; ///< Simplified ID (mapped from EOS_ProductUserId)
    std::vector<char> data; ///< Raw data (can be loaded into sf::Packet)
};

/// Event fired when a remote peer connects.
struct NetConnected {
    std::string userId;
};

/// Event fired when a remote peer disconnects.
struct NetDisconnected {
    std::string userId;
};

/// Event (variant of different network events)
using NetEvent = std::variant<NetPacket, NetConnected, NetDisconnected>;

class Net {
public:
    /// Constructs the Net facade and binds internal callbacks.
    Net();
    /// Destroys the Net facade and cleans up connections.
    ~Net();

    /// Starts the login process.
    void login();

    /// Logout and clear state.
    void logout();

    /// Starts hosting a lobby.
    void host(uint32_t maxPlayers, std::string& lobbyName);

    /// Finds and joins the first available lobby as a client.
    void connect(std::string& roomCode);

    /// Closes connections and releases resources.
    void close();

    //Leaves current lobby
    void leaveLobby();

    /// Sends raw data to a specific user (or host if client).
    /// @param packet Raw bytes to send.
    void send(const sf::Packet& packet);

    /// Call this every frame to tick EOS and process internal state.
    void fetch();

    /// Pop the next network event from the internal queue.
    /// @return Optional NetEvent (nullopt when queue empty).
    std::optional<NetEvent> next();

private:
    enum class Role {
        None,
        Host,
        Client
    };

    /// Reference to the global EOS manager instance.
    EOSManager& m_eosManager;

    /// Internal thread-safe event queue bridging EOS -> application.
    ThreadSafeQueue<NetEvent> m_eventQueue;

    /// Bind persistent callbacks (no-op until lobby exists).
    void BindCallbacks();

    /// Attach to a LobbyManager and subscribe to its delegates.
    void AttachToLobby(std::shared_ptr<LobbyManager> lobby);

    void PumpConnections(const std::shared_ptr<LobbyManager>& lobby);
    void ResetHandshakeState();

    /// Convert EOS id to string for the public API.
    std::string EOSIdToString(EOS_ProductUserId userId);

    /// Whether we've attached to the current lobby manager.
    bool m_attached = false;
    std::weak_ptr<LobbyManager> m_lobby;

    Role m_role = Role::None;
    bool m_clientHelloSent = false;
    std::unordered_set<std::string> m_hostHandshakePeers;

public:
    Delegate<void()> OnLobbySuccess; 
    Delegate<void(const std::string&)> OnPlayerConnected;
    Delegate<void(const std::string&)> OnPlayerDisconnected;
    Delegate<void(const std::string&, sf::Packet&)> OnPacketReceived;
    Delegate<void(const std::string&)> OnJoinFailed;
    Delegate<void()> OnLobbyLeft;
    Delegate<void()> OnHostLobbyLeft;
    Delegate<void()> OnLobbyDestroyed;

    void clearHandlers();

    std::string getLocalDisplayName();

    bool isLoggedIn() const; 
};