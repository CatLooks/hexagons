#pragma once
#include <iostream>
#include <vector>
#include <eos_common.h>
#include <eos_lobby.h>
#include <eos_p2p.h>
#include "networking_remake/P2PManager.hpp"
#include "networking_remake/Delegate.hpp"

class LobbyManager {
public:
	//Events you can subsribe to.
	Delegate<EOS_LobbyId> OnLobbyCreated;
	Delegate<EOS_LobbyId> OnLobbyJoined;
	Delegate<EOS_ProductUserId>  OnMemberJoined;
	Delegate<EOS_ProductUserId>  OnMemberLeft;

	bool isBusy = false;
	LobbyManager(EOS_HLobby handle, EOS_ProductUserId id, EOS_HP2P p2p);

	void CreateLobby();
	void FindLobby();

	~LobbyManager();

private:
	void HandleCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	void HandleFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	void HandleJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	void HandleMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);

	static void EOS_CALL OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	static void EOS_CALL OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	static void EOS_CALL OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	static void EOS_CALL OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);

	void JoinLobby(EOS_HLobbyDetails LobbyDetails);
	void RegisterMemberStatusNotifications();

	EOS_HLobby LobbyHandle = nullptr;
	EOS_HP2P P2PHandle = nullptr;
	EOS_ProductUserId LocalUserId = nullptr;
	EOS_ProductUserId HostId = nullptr;
	
	EOS_LobbyId LobbyId = nullptr;
	EOS_HLobbyDetails LobbyDetailsHandle = nullptr;
	EOS_HLobbySearch LobbySearchHandle = nullptr;
	EOS_NotificationId MemberStatusNotificationId = EOS_INVALID_NOTIFICATIONID;

	std::vector<EOS_ProductUserId> ExternalUsers;
	std::vector<std::shared_ptr<P2PManager>> P2PConnections;

	bool isHost = false;
};