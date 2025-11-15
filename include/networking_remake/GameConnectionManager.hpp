#pragma once
#include "networking_remake/EOSManager.hpp"

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

    // A reference to the core systems
    EOSManager& m_EosManager;

    // State specific to the connection flow
    EOS_ProductUserId m_LocalUserId = nullptr;
    bool m_IsHost = false;
};