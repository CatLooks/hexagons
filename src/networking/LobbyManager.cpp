#include "networking/LobbyManager.hpp"

LobbyManager::LobbyManager(EOS_HLobby handle, EOS_ProductUserId id, EOS_HP2P p2p)
	: LobbyHandle(handle), LocalUserId(id), P2PHandle(p2p) {
	std::cout << "[DEBUG] LocalUserId: " << LocalUserId << std::endl;
}

LobbyManager::~LobbyManager() {
	// Unregister the notification callback
	if (MemberStatusNotificationId != EOS_INVALID_NOTIFICATIONID) {
		EOS_Lobby_RemoveNotifyLobbyMemberStatusReceived(LobbyHandle, MemberStatusNotificationId);
	}
	//shared ptrs will auto clean up
}

void LobbyManager::CreateLobby(uint32_t maxPlayers, const char* lobbyCode) {
	isBusy = true;
	EOS_Lobby_CreateLobbyOptions options{};
	options.ApiVersion = EOS_LOBBY_CREATELOBBY_API_LATEST;
	options.LocalUserId = LocalUserId;
	options.MaxLobbyMembers = maxPlayers;
	options.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED;
	options.BucketId = lobbyCode;
	options.bPresenceEnabled = EOS_TRUE;
	options.bAllowInvites = EOS_TRUE;
	options.bDisableHostMigration = EOS_FALSE;
	options.bEnableRTCRoom = EOS_FALSE;
	options.AllowedPlatformIdsCount = 0;
	options.AllowedPlatformIds = nullptr;
	options.bCrossplayOptOut = EOS_FALSE;
	EOS_Lobby_CreateLobby(LobbyHandle, &options, this, OnCreateLobbyComplete);
}

void EOS_CALL LobbyManager::OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data) {
	if (auto Manager = static_cast<LobbyManager*>(Data->ClientData)) {
		Manager->HandleCreateLobbyComplete(Data);
	}
}

void LobbyManager::FindLobby() {
	isBusy = true;

	if (LobbySearchHandle) {
		EOS_LobbySearch_Release(LobbySearchHandle);
		LobbySearchHandle = nullptr;
	}

	EOS_Lobby_CreateLobbySearchOptions SearchOptions = {};
	SearchOptions.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
	SearchOptions.MaxResults = 1;

	if (EOS_Lobby_CreateLobbySearch(LobbyHandle, &SearchOptions, &LobbySearchHandle) != EOS_EResult::EOS_Success) {
		std::cerr << "[LobbyManager] Failed to create lobby search." << std::endl;
		isBusy = false;
		return;
	}

	

	EOS_Lobby_AttributeData SearchParameter{};
	SearchParameter.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
	SearchParameter.Key = EOS_LOBBY_SEARCH_BUCKET_ID;
	SearchParameter.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
	SearchParameter.Value.AsUtf8 = "default";

	EOS_LobbySearch_SetParameterOptions SetParamOptions = {};
	SetParamOptions.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	SetParamOptions.Parameter = &SearchParameter;
	SetParamOptions.ComparisonOp = EOS_EComparisonOp::EOS_CO_EQUAL;

	EOS_LobbySearch_SetParameter(LobbySearchHandle, &SetParamOptions);

	EOS_LobbySearch_FindOptions FindOptions = {};
	FindOptions.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
	FindOptions.LocalUserId = LocalUserId;

	EOS_LobbySearch_Find(LobbySearchHandle, &FindOptions, this, OnFindLobbyComplete);
}

void EOS_CALL LobbyManager::OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data) {
	if (auto Manager = static_cast<LobbyManager*>(Data->ClientData)) {
		Manager->HandleFindLobbyComplete(Data);
	}
}

void LobbyManager::JoinLobby(EOS_HLobbyDetails LobbyDetailsHandle) {
	isBusy = true;
	EOS_Lobby_JoinLobbyOptions JoinOptions = {};
	JoinOptions.ApiVersion = EOS_LOBBY_JOINLOBBY_API_LATEST;
	JoinOptions.LobbyDetailsHandle = LobbyDetailsHandle;
	JoinOptions.LocalUserId = LocalUserId;
	JoinOptions.bPresenceEnabled = EOS_TRUE;
	EOS_Lobby_JoinLobby(LobbyHandle, &JoinOptions, this, OnJoinLobbyComplete);
}

void EOS_CALL LobbyManager::OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data) {
	if (auto Manager = static_cast<LobbyManager*>(Data->ClientData)) {
		Manager->HandleJoinLobbyComplete(Data);
	}
}

void LobbyManager::RegisterMemberStatusNotifications() {
	EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions Options = {};
	Options.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;
	MemberStatusNotificationId = EOS_Lobby_AddNotifyLobbyMemberStatusReceived(LobbyHandle, &Options, this, OnMemberStatusChange);
}

void EOS_CALL LobbyManager::OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data) {
	if (auto Manager = static_cast<LobbyManager*>(Data->ClientData)) {
		Manager->HandleMemberStatusChange(Data);
	}
}

void LobbyManager::HandleCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data) {
	isBusy = false;
	if (Data->ResultCode == EOS_EResult::EOS_Success) {
		LobbyId = Data->LobbyId;
		std::cout << "[LobbyManager] Lobby created with ID: " << LobbyId << std::endl;
		RegisterMemberStatusNotifications();
		OnLobbyCreated.invoke(LobbyId);
	}
	else {
		std::cerr << "Failed to create lobby. Error code: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
	}
}

void LobbyManager::HandleFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data) {
	if (Data->ResultCode != EOS_EResult::EOS_Success) {
		std::cerr << "[LobbyManager] Lobby search failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
		isBusy = false;
		// Clean up the handle on failure.
		if (LobbySearchHandle) {
			EOS_LobbySearch_Release(LobbySearchHandle);
			LobbySearchHandle = nullptr;
		}
		return;
	}

	std::cout << "[LobbyManager] Lobby search successful." << std::endl;
	EOS_LobbySearch_GetSearchResultCountOptions CountOptions = {};
	CountOptions.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;

	uint32_t searchResultCount = EOS_LobbySearch_GetSearchResultCount(LobbySearchHandle, &CountOptions);

	if (searchResultCount == 0) {
		std::cout << "[LobbyManager] No lobbies found." << std::endl;
		isBusy = false;
	}
	else {
		EOS_HLobbyDetails LobbyDetailsHandle;
		EOS_LobbySearch_CopySearchResultByIndexOptions CopyOptions = {};
		CopyOptions.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
		CopyOptions.LobbyIndex = 0;

		if (EOS_LobbySearch_CopySearchResultByIndex(LobbySearchHandle, &CopyOptions, &LobbyDetailsHandle) == EOS_EResult::EOS_Success) {
			this->LobbyDetailsHandle = LobbyDetailsHandle; // Store the handle for later use
			JoinLobby(LobbyDetailsHandle);
		}
		else {
			std::cerr << "[LobbyManager] Failed to copy lobby details." << std::endl;
			isBusy = false;
		}
	}

	// ALWAYS release the search handle when the operation is complete.
	if (LobbySearchHandle) {
		EOS_LobbySearch_Release(LobbySearchHandle);
		LobbySearchHandle = nullptr;
	}
}

void LobbyManager::HandleJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data) {
	isBusy = false;
	if (Data->ResultCode == EOS_EResult::EOS_Success) {
		LobbyId = Data->LobbyId;
		std::cout << "[LobbyManager] Successfully joined lobby with ID: " << LobbyId << std::endl;
		RegisterMemberStatusNotifications();

		EOS_LobbyDetails_GetLobbyOwnerOptions OwnerOptions = {};
		OwnerOptions.ApiVersion = EOS_LOBBYDETAILS_GETLOBBYOWNER_API_LATEST;
		std::cout << "[DEBUG] LobbyDetailsHandle: " << LobbyDetailsHandle << std::endl;
		HostId = EOS_LobbyDetails_GetLobbyOwner(LobbyDetailsHandle, &OwnerOptions);
		std::cout << "[LobbyManager] Lobby owner ID: " << HostId << std::endl;
		LocalConnection = std::make_shared<P2PManager>(P2PHandle, LocalUserId, HostId);

		OnLobbyJoined.invoke(LobbyId); // Broadcast the event
	}
	else {
		std::cerr << "[LobbyManager] Failed to join lobby: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
	}
}

void LobbyManager::HandleMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data) {
	if (Data->TargetUserId == LocalUserId) return; // Ignore events about ourselves

	switch (Data->CurrentStatus) {
	case EOS_ELobbyMemberStatus::EOS_LMS_JOINED:
		std::cout << "[LobbyManager] Member joined: " << Data->TargetUserId << std::endl;
		ExternalUsers.push_back(Data->TargetUserId);
		P2PConnections.push_back(std::make_unique<P2PManager>(P2PHandle, LocalUserId, Data->TargetUserId));
		OnMemberJoined.invoke(Data->TargetUserId);
		break;
	case EOS_ELobbyMemberStatus::EOS_LMS_LEFT:
	case EOS_ELobbyMemberStatus::EOS_LMS_DISCONNECTED:
	case EOS_ELobbyMemberStatus::EOS_LMS_KICKED:
		std::cout << "[LobbyManager] Member left: " << Data->TargetUserId << std::endl;
		OnMemberLeft.invoke(Data->TargetUserId);
		// In a real game, you would find and remove the correct P2PManager from the vector here
		break;
	default:
		break;
	}
}

std::shared_ptr<P2PManager> LobbyManager::GetP2PConnection(EOS_ProductUserId peerId) {
	for (const auto& connection : P2PConnections) {
		if (connection->GetPeerId() == peerId) {
			return connection;
		}
	}
	return nullptr;
}