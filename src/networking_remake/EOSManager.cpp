#include "networking_remake/EOSManager.hpp"

void EOSManager::Tick() {
	EOS_Platform_Tick(platformHandle);
    if (GetLobbyManager()) {
        if (GetLobbyManager()->GetLocalConnection()) {
            GetLobbyManager()->GetLocalConnection()->ReceivePacket();
        }
        else if (GetLobbyManager()->GetPeerId()) {
			GetLobbyManager()->GetP2PConnection(GetLobbyManager()->GetPeerId())->ReceivePacket();
        }
    }
}

EOSManager::EOSManager() {
    EOSSdkConfig sdkConfig;
    PlatformManager platformManager;
    platformManager.InitializeEOSSdk(&sdkConfig);
    platformManager.CreatePlatformInstance(&sdkConfig);
    platformHandle = platformManager.GetPlatformHandle();

    logging = std::make_shared<LoggingManager>();
    logging->RegisterLoggingCallbackExample();
    logging->SetLogLevelVeryVerboseExample();

	auth = std::make_shared<AuthManager>(platformHandle);
    //hwid = std::make_shared<HWID>();

    lobbyHandle = EOS_Platform_GetLobbyInterface(platformHandle);
    p2pHandle = EOS_Platform_GetP2PInterface(platformHandle);
}

void EOSManager::CreateLobbyManager(EOS_ProductUserId localUserId) {
    if (!lobby) { // Ensure we only create it once
        lobby = std::make_shared<LobbyManager>(lobbyHandle, localUserId, p2pHandle);
    }
}

EOSManager::~EOSManager() {
    // Release the platform handle before shutting down
    if (platformHandle) {
        EOS_Platform_Release(platformHandle);
        platformHandle = nullptr;
    }
    EOS_Shutdown();
    std::cout << "[EOSManager] Shutdown complete." << std::endl;
}