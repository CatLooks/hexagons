#include "networking/GameConnectionManager.hpp"
#include <iostream>


GameConnectionManager::GameConnectionManager(EOSManager& eosManager)
    : m_EosManager(eosManager)
{
    auto auth = m_EosManager.GetAuthManager();
    if (auth) {
        auth->OnLoginSuccess.add([this](EOS_ProductUserId id) {
            this->OnLoginCompleted(id);
            });
    }
}

void GameConnectionManager::HostGame() {
    m_IsHost = true;
    std::cout << "[Connection] Command received: HostGame. Starting login..." << std::endl;
    //m_EosManager.GetHwidManager()->Login(m_EosManager.GetPlatformHandle());
    m_EosManager.GetAuthManager()->AccountPortalPersistentAuthLogin(); //DevAuthToolLogin(); //CreateDeviceId();
}

void GameConnectionManager::FindAndJoinGame() {
    m_IsHost = false;
    std::cout << "[Connection] Command received: FindAndJoinGame. Starting login..." << std::endl;
    //m_EosManager.GetHwidManager()->Login(m_EosManager.GetPlatformHandle());
    m_EosManager.GetAuthManager()->AccountPortalPersistentAuthLogin(); //DevAuthToolLogin(); //CreateDeviceId();
}

void GameConnectionManager::OnLoginCompleted(EOS_ProductUserId newLocalUserId) {
    std::cout << "[Connection] EVENT: Login successful." << std::endl;
    m_LocalUserId = newLocalUserId;

    m_EosManager.CreateLobbyManager(m_LocalUserId);
    auto lobby = m_EosManager.GetLobbyManager();
    if (!lobby) {
        std::cerr << "[Connection] CRITICAL: LobbyManager was not created!" << std::endl;
        return;
    }

    lobby->OnLobbyCreated.add([this](const std::string& lobbyId) {
        this->OnLobbyCreationCompleted(lobbyId);
        });
    lobby->OnLobbyJoined.add([this](const std::string& lobbyId) {
        this->OnLobbyJoinCompleted(lobbyId);
        });

    if (m_IsHost) {
        lobby->OnMemberJoined.add([this](EOS_ProductUserId memberId) {
            this->OnMemberJoined(memberId);
            });
    }
}

//TODO: actual implementation of further state transitions

void GameConnectionManager::OnLobbyCreationCompleted(const std::string& lobbyId) {
    std::cout << "[Connection] EVENT: Successfully created and joined lobby " << lobbyId << std::endl;
}

void GameConnectionManager::OnLobbyJoinCompleted(const std::string& lobbyId) {
    std::cout << "[Connection] EVENT: Successfully joined lobby " << lobbyId << std::endl;

    auto lobby = m_EosManager.GetLobbyManager();
    auto local = lobby->GetLocalConnection();

    local->OnMessageReceived.add([this](const std::string& message) {
        this->OnPacketReceived(const_cast<char*>(message.c_str()));
    });

    // Teraz wyœlij inicjaln¹ wiadomoœæ
    //local->SendPacket("Message from client: Joined lobby!", static_cast<uint32_t>(strlen("Message from client: Joined lobby!")));
}

void GameConnectionManager::OnMemberJoined(EOS_ProductUserId memberId) {
    std::cout << "[Connection] EVENT: Member joined: " << memberId << std::endl;
	auto lobby = m_EosManager.GetLobbyManager();
    lobby->GetP2PConnection(memberId)->OnMessageReceived.add([this](const std::string& message) {
        this->OnPacketReceived(const_cast<char*>(message.c_str()));
        });
}

void GameConnectionManager::OnPacketReceived(auto* message) { // Placeholder for actual message type
	std::cout << "[Connection] EVENT: Packet received: " << message << std::endl;
    auto lobby = m_EosManager.GetLobbyManager();
    //TODO: Actually process the contents
}