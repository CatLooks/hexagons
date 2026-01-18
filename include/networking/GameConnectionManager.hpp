#pragma once
#include "networking/EOSManager.hpp"

class GameConnectionManager {
public:
	// Takes a reference to the EOSManager to access core systems
    GameConnectionManager(EOSManager& eosManager);

    // High-level commands the rest of the game can issue
    void HostGame();
    void FindAndJoinGame();

private:
    // Event handlers for the entire connection flow
    void OnLoginCompleted(EOS_ProductUserId newLocalUserId);
    void OnLobbyCreationCompleted(const std::string& lobbyId);
    void OnLobbyJoinCompleted(const std::string& lobbyId);
	void OnMemberJoined(EOS_ProductUserId memberId);
	void OnPacketReceived(auto* message); // Placeholder for actual message type

    // A reference to the core systems
    EOSManager& m_EosManager;

    // State specific to the connection flow
    EOS_ProductUserId m_LocalUserId = nullptr;
    bool m_IsHost = false;
    bool m_SentPacket = false;
	bool m_ReceivedPacket = false;
};