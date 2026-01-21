#pragma once
#include <eos_common.h>
#include <memory>
#include "eos/Config.hpp"
#include "PlatformManager.hpp"
#include "LoggingManager.hpp"
#include "HWID.hpp"
#include "LobbyManager.hpp"
#include "P2PManager.hpp"
#include "AuthManager.hpp"


class EOSManager {
public:

	static EOSManager& GetInstance() {
		static EOSManager* instance = new EOSManager();
		return *instance;
	}

	void Tick();
	
	void CreateLobbyManager(EOS_ProductUserId localUserId);

	std::shared_ptr<AuthManager> GetAuthManager() const { return auth; }
	std::shared_ptr<HWID> GetHwidManager() const { return hwid; }
	std::shared_ptr<LobbyManager> GetLobbyManager() const { return lobby; }
	EOS_HPlatform GetPlatformHandle() const { return platformHandle; }
	EOS_HLobby GetLobbyHandle() const { return lobbyHandle; }
	EOS_HP2P GetP2PHandle() const { return p2pHandle; }

private:
	EOSManager();
	~EOSManager();

	EOS_HPlatform platformHandle = nullptr;
	EOS_HLobby lobbyHandle = nullptr;
	EOS_HP2P p2pHandle = nullptr;

	std::shared_ptr<AuthManager> auth = nullptr;
	std::shared_ptr<HWID> hwid = nullptr;
	std::shared_ptr<LoggingManager> logging = nullptr;
	std::shared_ptr<LobbyManager> lobby = nullptr;
};