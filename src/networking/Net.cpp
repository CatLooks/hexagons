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

void Net::leaveLobby() {
    auto lobby = m_eosManager.GetLobbyManager();
    
    if (lobby) {
        if (m_role == Role::Host) {
            lobby->DestroyLobby();
        }
        else if (m_role == Role::Client) {
            lobby->LeaveLobby();
        }
    }

    // Force Cleanup (Always runs)
    close();

    // Notify UI (MenuSystem hears this -> calls startMenu->reset() -> page switches)
    OnLobbyLeft.invoke();
}

/// Start the login process.
void Net::login() {
    auto auth = m_eosManager.GetAuthManager();
    if (auth) {
        auth->AccountPortalPersistentAuthLogin();
        //auth->CreateDeviceId(); //temp - sorry
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
}

/// Find and join a lobby as a client.
void Net::connect(std::string& roomCode) {
    ResetHandshakeState();
    m_role = Role::Client;

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
void Net::send(const sf::Packet& packet) {
    auto lobby = m_eosManager.GetLobbyManager();
    if (!lobby) return;

    // If host: broadcast to peers.
    if (m_role == Role::Host) {
        const std::vector<std::shared_ptr<P2PManager>> peers = lobby->GetAllP2PConnections();
        if (!peers.empty()) {
            for (const auto& peer : peers) {
                if (peer) {
                    peer->SendPacket(packet);
                }
            }
            return;
        }
    }
    // If client: use LocalConnection.
    else {
        if (auto local = lobby->GetLocalConnection()) {
            local->SendPacket(packet);
            return;
        }
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
        OnPlayerConnected.invoke(ev.userId);
    });

     auto local = lobby->GetLocalConnection();
    if (local) {
        local->OnMessageReceived.add([this](sf::Packet packet) {
            OnPacketReceived.invoke(EOSIdToString(nullptr), packet);
        });
    }

    // Member left -> NetDisconnected
    lobby->OnMemberLeft.add([this](EOS_ProductUserId userId) {
        NetDisconnected ev;
        ev.userId = EOSIdToString(userId);
        m_eventQueue.push(ev);
		OnPlayerDisconnected.invoke(ev.userId);
    });

    lobby->OnLobbyJoined.add([this, lobby](EOS_LobbyId id) {
        auto local = lobby->GetLocalConnection();
        if (local) {
            local->OnMessageReceived.add([this](sf::Packet packet) {
                NetPacket pkt;
                pkt.data.assign(
                    static_cast<const char*>(packet.getData()),
                    static_cast<const char*>(packet.getData()) + packet.getDataSize()
                );
                pkt.senderId = EOSIdToString(nullptr);
                m_eventQueue.push(pkt);
               OnPacketReceived.invoke(pkt.senderId, packet);  
            });
        }
        OnLobbySuccess.invoke();

    });

    lobby->OnLobbyCreated.add([this, lobby](EOS_LobbyId) {
        OnLobbySuccess.invoke();
    });

    // Leaving delegates
    lobby->OnLobbyLeft.add([this](EOS_LobbyId) {
        // local user left (client case)
        close();
        OnLobbyLeft.invoke();
    });

    lobby->OnHostLobbyLeft.add([this](EOS_LobbyId) {
        // local user destroyed lobby (host case)
        close();
        OnHostLobbyLeft.invoke();
    });

    lobby->OnLobbyDestroyed.add([this](EOS_LobbyId) {
        // clients see lobby destroyed
        close();
        OnLobbyDestroyed.invoke();
    });

    lobby->OnMemberJoined.add([this, lobby](EOS_ProductUserId userId) {
        auto p2p = lobby->GetP2PConnection(userId);
        const std::string peerKey = EOSIdToString(userId);
        if (p2p) {
            p2p->OnMessageReceived.add([this, userId, p2p, peerKey](sf::Packet packet) {
                NetPacket pkt;
                pkt.data.assign(
                    static_cast<const char*>(packet.getData()),
                    static_cast<const char*>(packet.getData()) + packet.getDataSize()
                );
                pkt.senderId = EOSIdToString(nullptr);
                m_eventQueue.push(pkt);
				OnPacketReceived.invoke(peerKey, packet);
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

void Net::clearHandlers() {
    // Only clear handlers that are specific to a specific game session
    OnPlayerConnected.clear();
    OnPlayerDisconnected.clear();
    OnPacketReceived.clear();
    OnLobbySuccess.clear();

    // OnLobbyLeft.clear();           
    // OnHostLobbyLeft.clear();   
    // OnLobbyDestroyed.clear();
}

std::string Net::getLocalDisplayName() {
    auto auth = m_eosManager.GetAuthManager();
    if (auth) {
        return auth->GetDisplayName();
    }
    return "Unknown Player";
}

bool Net::isLoggedIn() const {
	//NOTE: dlaczego po wylogowaniu pozostaje logged in true? - Pytanie do Kamila za 1000 pktów
    return m_eosManager.GetAuthManager()->IsLoggedIn();
}