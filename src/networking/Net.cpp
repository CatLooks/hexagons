#include "networking/Net.hpp"
#include <iostream>

/// Constructs the Net facade and binds callbacks.
Net::Net() : m_eosManager(EOSManager::GetInstance()) {
    ResetHandshakeState();
    BindCallbacks();
}

/// Destructor cleans up resources.
Net::~Net() {
    close();
	logout();
}

/// Start the login process.
void Net::login() {
    auto auth = m_eosManager.GetAuthManager();
    if (auth) {
        auth->AccountPortalPersistentAuthLogin();
        std::cout << "[Net] Login initiated..." << std::endl;
    }
}

/// Logout and clear state.
void Net::logout() {
    auto auth = m_eosManager.GetAuthManager();
    if (auth) {
        auth->Logout();
        std::cout << "[Net] Logout initiated..." << std::endl;
    }
    close();
}

/// Start hosting a lobby.
void Net::host(uint32_t maxPlayers, std::string& lobbyCode) {
    //login();
    EOS_ProductUserId localId = m_eosManager.GetAuthManager()->GetLocalUserId();

    ResetHandshakeState();
    m_role = Role::Host;

    while(!localId) {
        m_eosManager.Tick();
        localId = m_eosManager.GetAuthManager()->GetLocalUserId();
	}

    m_eosManager.CreateLobbyManager(localId);
    if (auto lobby = m_eosManager.GetLobbyManager()) {
        AttachToLobby(lobby);
        lobby->CreateLobby(maxPlayers, lobbyCode);
        std::cout << "[Net] Hosting initiated..." << std::endl;
    }
    else {
        std::cerr << "[Net] Failed to get LobbyManager after creation." << std::endl;
    }
    /*
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
    */
}

/// Find and join a lobby as a client.
void Net::connect(std::string& roomCode) {
    ResetHandshakeState();
    m_role = Role::Client;

    //login();
    EOS_ProductUserId localId = m_eosManager.GetAuthManager()->GetLocalUserId();

    while (!localId) {
        m_eosManager.Tick();
        localId = m_eosManager.GetAuthManager()->GetLocalUserId();
    }

    m_eosManager.CreateLobbyManager(localId);
    if (auto lobby = m_eosManager.GetLobbyManager()) {
        AttachToLobby(lobby);
        lobby->FindLobby(roomCode);
        std::cout << "[Net] Searching for lobby..." << std::endl;
    }
    else {
        std::cerr << "[Net] Failed to get LobbyManager after creation." << std::endl;
    }

    /*
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
    */
}

/// Close connections and stop listening to lobby events.
void Net::close() {
    m_lobby.reset();
    m_attached = false;
    ResetHandshakeState();
    m_role = Role::None;
}

/// Tick EOS and ensure we are attached to the LobbyManager when it becomes available.
void Net::fetch() {
    m_eosManager.Tick();

    // Attach to lobby when it is created by EOSManager.
    auto lobby = m_lobby.lock();
    if (!lobby || !m_attached) {
        if (auto eosLobby = m_eosManager.GetLobbyManager()) {
            AttachToLobby(eosLobby);
            lobby = eosLobby;
        }
    }

    if (lobby) {
        PumpConnections(lobby);
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
		std::vector<std::shared_ptr<P2PManager>> peers = lobby->GetAllP2PConnections();
        for (const auto& peer : peers) {
            std::string message(data.begin(), data.end());
            peer->SendString(message);
        }
		return;
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
    lobby->OnMemberJoined.add([this](EOS_ProductUserId userId) {
        NetConnected ev;
        ev.userId = EOSIdToString(userId);
        m_eventQueue.push(ev);
    });

    // Member left -> NetDisconnected
    lobby->OnMemberLeft.add([this](EOS_ProductUserId userId) {
        NetDisconnected ev;
        ev.userId = EOSIdToString(userId);
        m_eventQueue.push(ev);
    });

    // When joining a lobby, LobbyManager creates a LocalConnection; hook its message delegate.
    lobby->OnLobbyJoined.add([this, lobby](EOS_LobbyId id) {
        auto local = lobby->GetLocalConnection();
        if (local) {
            local->OnMessageReceived.add([this](char* data) {
                NetPacket pkt;
                std::string s(data);
                pkt.data.assign(s.begin(), s.end());
                pkt.senderId = EOSIdToString(nullptr);
                m_eventQueue.push(pkt);
                });

            if (m_role == Role::Client && !m_clientHelloSent) {
                local->SendString("[Net] ClientHello");
                m_clientHelloSent = true;
            }
        }
        });

    // Host-created lobby: peers will be reported via OnMemberJoined; additional hooking can be done in future.
    lobby->OnLobbyCreated.add([this, lobby](EOS_LobbyId id) {
        // no-op for now
        });

    lobby->OnMemberJoined.add([this, lobby](EOS_ProductUserId userId) {
        auto p2p = lobby->GetP2PConnection(userId);
        const std::string peerKey = EOSIdToString(userId);
        if (p2p) {
            p2p->OnMessageReceived.add([this, userId, p2p, peerKey](char* data) {
                NetPacket pkt;
                std::string s(data);
                pkt.data.assign(s.begin(), s.end());
                pkt.senderId = peerKey;
                m_eventQueue.push(pkt);

                if (m_role == Role::Host && m_hostHandshakePeers.insert(peerKey).second) {
                    p2p->SendString("[Net] HostHello");
                }
                });
        }
        });
}


void Net::PumpConnections(const std::shared_ptr<LobbyManager>& lobby) {
    if (!lobby) {
        return;
    }

    if (auto local = lobby->GetLocalConnection()) {
        while (local->ReceivePacket()) {}
    }

    const auto peers = lobby->GetAllP2PConnections();
    for (const auto& peer : peers) {
        if (!peer) {
            continue;
        }
        while (peer->ReceivePacket()) {}
    }
}

void Net::ResetHandshakeState() {
    m_clientHelloSent = false;
    m_hostHandshakePeers.clear();
}

/// Convert EOS_ProductUserId to string. Fallbacks to pointer address representation.
std::string Net::EOSIdToString(EOS_ProductUserId userId) {
    if (!userId) return std::string("(null)");
    const void* p = static_cast<const void*>(userId);
    char buf[32];
    snprintf(buf, sizeof(buf), "%p", p);
    return std::string(buf);
}