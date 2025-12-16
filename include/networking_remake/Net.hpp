#pragma once
#include <variant>
#include <vector>
#include <string>
#include <memory>
#include <optional>
#include <unordered_set>

#include "networking_remake/EOSManager.hpp"
#include "networking_remake/threadsafe_queue.hpp"

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
    void host();

    /// Finds and joins the first available lobby as a client.
    void connect();

    /// Closes connections and releases resources.
    void close();

    /// Sends raw data to a specific user (or host if client).
    /// @param data Raw bytes to send.
    /// @param targetId Optional target id string. Empty => default routing (host/client semantics).
    void send(const std::vector<char>& data, const std::string& targetId = "");


    /// Overload for sending a plain string.
    void send(const std::string& msg) {
        std::vector<char> data(msg.begin(), msg.end());
        send(data);
    }

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
};