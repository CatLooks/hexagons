#pragma once
#include <iostream>
#include <vector>
#include "eos/P2PManager.hpp"
#include <eos_common.h>
#include <eos_lobby.h>
#include <eos_p2p.h>

class LobbyManager {
public:
	bool isBusy = false;
	LobbyManager(EOS_HLobby* handle, EOS_ProductUserId* id, EOS_HP2P* p2p) : LobbyHandle(handle), LocalUserId(id), P2PHandle(p2p) {}
	
	void CreateLobby();
	void SetLobbyId(EOS_LobbyId* id) { LobbyId = id; };
	bool isInLobby() const { return LobbyId != nullptr; };
	void setHost(bool host) { isHost = host; };

	void FindLobby();
	void JoinLobby();

	void RegisterMemberStatusChange();
	~LobbyManager() {
		delete LobbyId;
		delete LobbySearchHandle;
		for (EOS_ProductUserId* user : ExternalUsers) {
			delete user;
		}
		for (P2PManager* p2p : P2PConnections) {
			delete p2p;
		}
		EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived(*LobbyHandle, *MemberStatusChangeNotificationId);
		delete MemberStatusChangeNotificationId;
	}
private:
	static void EOS_CALL OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	static void EOS_CALL OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	static void EOS_CALL OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	static void EOS_CALL OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);

	EOS_HLobby* LobbyHandle = nullptr;
	EOS_HP2P* P2PHandle = nullptr;
	EOS_ProductUserId* LocalUserId = nullptr;
	EOS_ProductUserId* HostId = nullptr;
	
	EOS_LobbyId* LobbyId = nullptr;
	EOS_LobbyDetailsHandle* LobbyDetailsHandle = nullptr;
	EOS_HLobbySearch* LobbySearchHandle = nullptr;
	EOS_NotificationId* MemberStatusChangeNotificationId = 0;

	std::vector<EOS_ProductUserId*> ExternalUsers;
	std::vector<P2PManager*> P2PConnections;

	bool isHost = false;
};