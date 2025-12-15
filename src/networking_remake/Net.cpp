#include "networking_remake/Net.hpp"
#include <iostream>

/// Constructs the Net facade and binds callbacks.
Net::Net() : m_eosManager(EOSManager::GetInstance()) {
    BindCallbacks();
}

/// Destructor cleans up resources.
Net::~Net() {
    close();
}

/// Start hosting a lobby.
void Net::host() {
    EOS_ProductUserId localId = m_eosManager.GetAuthManager()->GetLocalUserId();
    if (localId) {
        m_eosManager.CreateLobbyManager(localId);
        if (auto lobby = m_eosManager.GetLobbyManager()) {
            AttachToLobby(lobby);
            lobby->CreateLobby();
            std::cout << "[Net] Hosting initiated..." << std::endl;
        }
        else {
            std::cerr << "[Net] Failed to get LobbyManager after creation." << std::endl;
        }
    }
    else {
        std::cerr << "[Net] Cannot host: Not logged in." << std::endl;
    }
}

/// Find and join a lobby as a client.
void Net::connect() {
    EOS_ProductUserId localId = m_eosManager.GetAuthManager()->GetLocalUserId();
    if (localId) {
        m_eosManager.CreateLobbyManager(localId);
        if (auto lobby = m_eosManager.GetLobbyManager()) {
            AttachToLobby(lobby);
            lobby->FindLobby();
            std::cout << "[Net] Searching for lobby..." << std::endl;
        }
        else {
            std::cerr << "[Net] Failed to get LobbyManager after creation." << std::endl;
        }
    }
}

/// Close connections and stop listening to lobby events.
void Net::close() {
    m_lobby.reset();
    m_attached = false;
}

/// Tick EOS and ensure we are attached to the LobbyManager when it becomes available.
void Net::fetch() {
    m_eosManager.Tick();

    // Attach to lobby when it is created by EOSManager.
    if (!m_attached) {
        if (auto lobby = m_eosManager.GetLobbyManager()) {
            AttachToLobby(lobby);
        }
    }
}

/// Pop next network event from internal queue.
std::optional<NetEvent> Net::next() {
    return m_eventQueue.pop();
}

/// Send raw data. Uses LocalConnection when present as a default route.
void Net::send(const std::vector<char>& data, const std::string& targetId) {
    auto lobby = m_eosManager.GetLobbyManager();
    if (!lobby) return;

    // Default routing: client -> host via LocalConnection.
    if (targetId.empty()) {
        if (auto local = lobby->GetLocalConnection()) {
            std::string message(data.begin(), data.end());
            local->SendString(message);
            return;
        }

        // Host broadcasting is not implemented here; requires LobbyManager to expose peer list.
    }
    else {
        // Targeted send by string id is not supported without id -> EOS_ProductUserId mapping.
    }

    // Fallback: try sending through local connection if available.
    if (auto local = lobby->GetLocalConnection()) {
        std::string message(data.begin(), data.end());
        local->SendString(message);
    }
}

/// Bind persistent callbacks (placeholder). Actual attachment happens in AttachToLobby.
void Net::BindCallbacks() {
    // intentionally empty
}

/// Attach to a LobbyManager and convert its delegates to NetEvent pushed into the internal queue.
void Net::AttachToLobby(std::shared_ptr<LobbyManager> lobby) {
    if (!lobby) return;

    if (auto existing = m_lobby.lock()) {
        if (existing == lobby) {
            m_attached = true;
            return;
        }
    }

    m_lobby = lobby;
    m_attached = true;

    // Member joined -> NetConnected
    lobby->OnMemberJoined.Add([this](EOS_ProductUserId userId) {
        NetConnected ev;
        ev.userId = EOSIdToString(userId);
        m_eventQueue.push(ev);
    });

    // Member left -> NetDisconnected
    lobby->OnMemberLeft.Add([this](EOS_ProductUserId userId) {
        NetDisconnected ev;
        ev.userId = EOSIdToString(userId);
        m_eventQueue.push(ev);
    });

    // When joining a lobby, LobbyManager creates a LocalConnection; hook its message delegate.
    lobby->OnLobbyJoined.Add([this, lobby](EOS_LobbyId id) {
        auto local = lobby->GetLocalConnection();
        if (local) {
            local->OnMessageReceived.Add([this](char* data) {
                NetPacket pkt;
                std::string s(data);
                pkt.data.assign(s.begin(), s.end());
                pkt.senderId = EOSIdToString(nullptr); // host id unknown here
                m_eventQueue.push(pkt);
            });
        }
    });

    // Host-created lobby: peers will be reported via OnMemberJoined; additional hooking can be done in future.
    lobby->OnLobbyCreated.Add([this, lobby](EOS_LobbyId id) {
        // no-op for now
    });
}

/// Convert EOS_ProductUserId to string. Fallbacks to pointer address representation.
std::string Net::EOSIdToString(EOS_ProductUserId userId) {
    if (!userId) return std::string("(null)");
    const void* p = static_cast<const void*>(userId);
    char buf[32];
    snprintf(buf, sizeof(buf), "%p", p);
    return std::string(buf);
}