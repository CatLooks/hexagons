#pragma once
#include <iostream>
#include <vector>
#include <eos_common.h>
#include <algorithm>
#include <eos_lobby.h>
#include <eos_p2p.h>
#include "networking/P2PManager.hpp"
#include "templated/delegate.hpp"

class LobbyManager {
public:
	//Events you can subsribe to.
	Delegate<void(EOS_LobbyId)> OnLobbyCreated;
	Delegate<void(EOS_LobbyId)> OnLobbyJoined;
	Delegate<void(EOS_ProductUserId)>  OnMemberJoined;
	Delegate<void(EOS_ProductUserId)>  OnMemberLeft;
	Delegate<void(EOS_LobbyId)> OnLobbyLeft;
	Delegate<void(EOS_LobbyId)> OnLobbyDestroyed;
	Delegate<void(EOS_LobbyId)> OnHostLobbyLeft;
	Delegate<void(const std::string&)> OnLobbyJoinFailed;

	bool isBusy = false;
	LobbyManager(EOS_HLobby handle, EOS_ProductUserId id, EOS_HP2P p2p);

	void CreateLobby(uint32_t maxPlayers, std::string& lobbyCode);
	void FindLobby(std::string& roomCode);

	void LeaveLobby();
	void DestroyLobby();

	std::vector<std::shared_ptr<P2PManager>> GetAllP2PConnections() const { return P2PConnections; }
	std::shared_ptr<P2PManager> GetP2PConnection(EOS_ProductUserId peerId);
	std::shared_ptr<P2PManager> GetLocalConnection() { return LocalConnection; }
	EOS_ProductUserId GetPeerId() const { return ExternalUsers.empty() ? nullptr : ExternalUsers[0]; }

	~LobbyManager();

private:
	void HandleCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	void HandleFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	void HandleJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	void HandleMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);
	void HandleLeaveLobbyComplete(const EOS_Lobby_LeaveLobbyCallbackInfo* Data);
	void HandleDestroyLobbyComplete(const EOS_Lobby_DestroyLobbyCallbackInfo* Data);

	static void EOS_CALL OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data);
	static void EOS_CALL OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data);
	static void EOS_CALL OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data);
	static void EOS_CALL OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data);
	static void EOS_CALL OnLeaveLobbyComplete(const EOS_Lobby_LeaveLobbyCallbackInfo* Data);
	static void EOS_CALL OnDestroyLobbyComplete(const EOS_Lobby_DestroyLobbyCallbackInfo* Data);

	void JoinLobby(EOS_HLobbyDetails LobbyDetails);
	void RegisterMemberStatusNotifications();
	void UnregisterMemberStatusNotifications();

	//Required handles and IDs
	EOS_HLobby LobbyHandle = nullptr;
	EOS_HP2P P2PHandle = nullptr;
	EOS_ProductUserId LocalUserId = nullptr;
	EOS_ProductUserId HostId = nullptr;

	//Lobby-related handles and IDs
	std::string LobbyIdStr;
	EOS_LobbyId LobbyId = nullptr;
	EOS_HLobbyDetails LobbyDetailsHandle = nullptr;
	EOS_HLobbySearch LobbySearchHandle = nullptr;
	EOS_NotificationId MemberStatusNotificationId = EOS_INVALID_NOTIFICATIONID;
	std::string LastSearchCode;

	//Client connection manager for communication with host
	std::shared_ptr<P2PManager> LocalConnection = nullptr;

	//Host connection manager for mass communication with peers
	std::vector<EOS_ProductUserId> ExternalUsers;
	std::vector<std::shared_ptr<P2PManager>> P2PConnections;

	//duh.
	bool isHost = false;
	//bool receivedPacket = false;
};