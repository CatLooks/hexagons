#include "eos/EOSManager.hpp"

void EOSManager::Tick() {
	EOS_Platform_Tick(*platformHandle);

	switch (state) {

	case INIT:
		if (hwid->GetLocalUserId() != nullptr) {
			localUserId = new EOS_ProductUserId(hwid->GetLocalUserId());
			if (EOS_ProductUserId_IsValid(*localUserId) == EOS_FALSE) {
				std::cout << "[EOSManager] Invalid Product User ID before deleting HWID!" << std::endl;
			}
			delete hwid;
			if (EOS_ProductUserId_IsValid(*localUserId) == EOS_FALSE) {
				std::cout << "[EOSManager] Invalid Product User ID after deleting HWID!" << std::endl;
			}
			lobby = new LobbyManager(lobbyHandle, localUserId, p2pHandle);
			state = LOGGED_IN;
		}
		break;

	case LOGGED_IN:
		if (isHost) {
			lobby->CreateLobby();
		}
		else {
			lobby->FindLobby();
		}
		state = JOINING_LOBBY;
		break;

	case JOINING_LOBBY:
		if (isHost) {
			if (!lobby->isBusy) {
				state = IN_LOBBY;
			}
		}
		else {
			if (!lobby->isBusy) {
				state = IN_LOBBY;
			}
		}
		break;

	case IN_LOBBY:
		break;
	}
}