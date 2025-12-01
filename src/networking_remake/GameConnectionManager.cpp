#include "networking_remake/GameConnectionManager.hpp"
#include <iostream>


GameConnectionManager::GameConnectionManager(EOSManager& eosManager)
    : m_EosManager(eosManager)
{
    /*
    auto hwid = m_EosManager.GetHwidManager();
    if (hwid) {
        hwid->OnLoginSuccess.Add([this](EOS_ProductUserId id) {
            this->OnLoginCompleted(id);
            });
    }*/

    auto auth = m_EosManager.GetAuthManager();
    if (auth) {
        auth->OnLoginSuccess.Add([this](EOS_ProductUserId id) {
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

    lobby->OnLobbyCreated.Add([this](const std::string& lobbyId) {
        this->OnLobbyCreationCompleted(lobbyId);
        });
    lobby->OnLobbyJoined.Add([this](const std::string& lobbyId) {
        this->OnLobbyJoinCompleted(lobbyId);
        });

    if (m_IsHost) {
        std::cout << "[Connection] Issuing command: CreateLobby" << std::endl;
        lobby->CreateLobby();
        lobby->OnMemberJoined.Add([this](EOS_ProductUserId memberId) {
            this->OnMemberJoined(memberId);
            });
    }
    else {
        std::cout << "[Connection] Issuing command: FindLobby" << std::endl;
        lobby->FindLobby();
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

    local->OnMessageReceived.Add([this](char* message) {
        this->OnPacketReceived(message);
    });

    // Teraz wyœlij inicjaln¹ wiadomoœæ
    local->SendString("Message from client: Joined lobby!");
}

void GameConnectionManager::OnMemberJoined(EOS_ProductUserId memberId) {
    std::cout << "[Connection] EVENT: Member joined: " << memberId << std::endl;
	auto lobby = m_EosManager.GetLobbyManager();
    lobby->GetP2PConnection(memberId)->OnMessageReceived.Add([this](char* message) {
        this->OnPacketReceived(message);
        });
}

void GameConnectionManager::OnPacketReceived(char* message){
	std::cout << "[Connection] EVENT: Packet received: " << message << std::endl;
    auto lobby = m_EosManager.GetLobbyManager();
    if (!lobby->HasReceivedPacket()) {
        lobby->SetReceivedPacket(true);
        if (m_IsHost) {
            auto peer = lobby->GetP2PConnection(lobby->GetPeerId());
            peer->SendString("Message from host: Packet received!");
        }
    }
}