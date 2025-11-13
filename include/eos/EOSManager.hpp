#pragma once
#include <eos_common.h>
#include "Config.hpp"
#include "PlatformManager.hpp"
#include "LoggingManager.hpp"
#include "HWID.hpp"
#include "LobbyManager.hpp"
#include "P2PManager.hpp"

class EOSManager {
public:
	LoggingManager* logging;
	LobbyManager* lobby;

	EOSManager() {
		state = INIT;
		EOSSdkConfig* sdkConfig = new EOSSdkConfig();
		PlatformManager* platformManager = new PlatformManager();
		platformManager->InitializeEOSSdk(sdkConfig);
		platformManager->CreatePlatformInstance(sdkConfig);
		delete sdkConfig;
		platformHandle = platformManager->GetPlatformHandle();
		delete platformManager;

		logging = new LoggingManager();
		logging->RegisterLoggingCallbackExample();
		logging->SetLogLevelVeryVerboseExample();

		hwid = new HWID();
		hwid->Login(platformHandle);

		lobbyHandle = new EOS_HLobby(EOS_Platform_GetLobbyInterface(*platformHandle));
		p2pHandle = new EOS_HP2P(EOS_Platform_GetP2PInterface(*platformHandle));

		int temp;
		std::cout << "Host: 1, Client: 2" << std::endl;
		std::cin >> temp;
		temp == 1 ? isHost = true : isHost = false;
	}

	void Tick();

	EOS_HPlatform* GetPlatform() const{ return platformHandle; }

	~EOSManager() {
		delete lobby;
		delete logging;
		delete localUserId;
		delete p2pHandle;
		delete lobbyHandle;
		EOS_Platform_Release(*platformHandle);
		delete platformHandle;
		EOS_Shutdown();
	}
private:
	EOS_HPlatform* platformHandle = nullptr;
	EOS_HLobby* lobbyHandle = nullptr;
	EOS_HP2P* p2pHandle = nullptr;
	EOS_ProductUserId* localUserId = nullptr;
	HWID* hwid = nullptr;

	enum State {
		INIT,
		LOGGED_IN,
		JOINING_LOBBY,
		IN_LOBBY
	} state;
	
	bool isHost;
};