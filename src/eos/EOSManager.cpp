#include "eos/EOSManager.hpp"

void EOSManager::Tick() {
	EOS_Platform_Tick(*platformHandle);

	switch (state) {

	case INIT:
		if (hwid->GetLocalUserId() != nullptr) {
			localUserId = hwid->GetLocalUserId();
			delete hwid;
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