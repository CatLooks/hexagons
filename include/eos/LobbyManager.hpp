#pragma once
#include <vector>
#include <eos_common.h>
#include <eos_lobby.h>
#include <iostream>
#include <memory>

class LobbyManager {
public:
	bool isBusy = false;
	LobbyManager(EOS_HLobby* handle, std::shared_ptr<EOS_ProductUserId> id) : LobbyHandle(handle), UserId(id) {}
	
	void CreateLobby();
	void SetLobbyId(EOS_LobbyId* id) { LobbyId = id; };
	bool isInLobby() const { return LobbyId != nullptr; }
	std::shared_ptr<EOS_ProductUserId> GetSecondUserId() const { return SecondUserId; }

	void FindLobby();
	void JoinLobby();

	void RegisterMemberStatusChange();

	~LobbyManager() {
		if (MemberStatusChangeNotificationId != 0) {
			EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived(*LobbyHandle, *MemberStatusChangeNotificationId);
		}
		delete LobbyId;
		if (LobbySearchHandle) {
			EOS_LobbySearch_Release(*LobbySearchHandle);
		}
		if (LobbyDetailsHandle) {
			EOS_LobbyDetails_Release(*LobbyDetailsHandle);
		}
		delete LobbySearchHandle;
		delete LobbyDetailsHandle;
		delete LobbyHandle;
		delete MemberStatusChangeNotificationId;
	}
private:
	static void EOS_CALL OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	static void EOS_CALL OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	static void EOS_CALL OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	static void EOS_CALL OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);

	EOS_HLobby* LobbyHandle = nullptr;
	std::shared_ptr<EOS_ProductUserId> UserId = nullptr;
	std::shared_ptr<EOS_ProductUserId> SecondUserId = nullptr;
	EOS_LobbyId* LobbyId = nullptr;
	EOS_HLobbySearch* LobbySearchHandle = nullptr;
	EOS_HLobbyDetails* LobbyDetailsHandle = nullptr;
	EOS_NotificationId* MemberStatusChangeNotificationId = 0;
};