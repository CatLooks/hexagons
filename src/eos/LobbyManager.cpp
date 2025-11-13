#include "eos/LobbyManager.hpp"

void LobbyManager::CreateLobby() {
	isBusy = true;
	EOS_Lobby_CreateLobbyOptions options{};
	options.ApiVersion = EOS_LOBBY_CREATELOBBY_API_LATEST;
	options.LocalUserId = *LocalUserId;
	options.MaxLobbyMembers = 4;
	options.PermissionLevel = EOS_ELobbyPermissionLevel::EOS_LPL_PUBLICADVERTISED;
	options.BucketId = "default";
	options.bPresenceEnabled = EOS_TRUE;
	options.bAllowInvites = EOS_TRUE;
	options.bDisableHostMigration = EOS_FALSE;
	options.bEnableRTCRoom = EOS_FALSE;
	options.AllowedPlatformIdsCount = 0;
	options.AllowedPlatformIds = nullptr;
	options.bCrossplayOptOut = EOS_FALSE;
	EOS_Lobby_CreateLobby(*LobbyHandle, &options, this, OnCreateLobbyComplete);
}

void LobbyManager::OnCreateLobbyComplete(const EOS_Lobby_CreateLobbyCallbackInfo* Data)
{
	LobbyManager* Manager = static_cast<LobbyManager*>(Data->ClientData);
	if (Data->ResultCode == EOS_EResult::EOS_Success) {
		Manager->SetLobbyId(const_cast<EOS_LobbyId*>(&Data->LobbyId));
		std::cout << "Lobby created with ID: " << Data->LobbyId << std::endl;
		Manager->RegisterMemberStatusChange();
	}
	else {
		std::cerr << "Failed to create lobby. Error code: " << static_cast<int>(Data->ResultCode) << std::endl;
	}
	Manager->isBusy = false;
}

void LobbyManager::FindLobby() {
	isBusy = true;
	EOS_Lobby_CreateLobbySearchOptions SearchOptions = {};
	SearchOptions.ApiVersion = EOS_LOBBY_CREATELOBBYSEARCH_API_LATEST;
	SearchOptions.MaxResults = 10;
	EOS_EResult createResult = EOS_Lobby_CreateLobbySearch(*LobbyHandle, &SearchOptions, LobbySearchHandle);
	if (createResult != EOS_EResult::EOS_Success) {
		std::cerr << "[LobbyManager] Failed to create lobby search: " << EOS_EResult_ToString(createResult) << std::endl;
		return;
	}

	EOS_Lobby_AttributeData SearchParameter{};
	SearchParameter.ApiVersion = EOS_LOBBY_ATTRIBUTEDATA_API_LATEST;
	SearchParameter.Key = EOS_LOBBY_SEARCH_BUCKET_ID;
	SearchParameter.ValueType = EOS_ELobbyAttributeType::EOS_AT_STRING;
	SearchParameter.Value.AsUtf8 = "default";

	EOS_LobbySearch_SetParameterOptions SetParamOptions = {};
	SetParamOptions.ApiVersion = EOS_LOBBYSEARCH_SETPARAMETER_API_LATEST;
	SetParamOptions.ComparisonOp = EOS_EComparisonOp::EOS_CO_EQUAL;
	SetParamOptions.Parameter = &SearchParameter;
	EOS_LobbySearch_SetParameter(*LobbySearchHandle, &SetParamOptions);

	EOS_LobbySearch_FindOptions FindOptions = {};
	FindOptions.ApiVersion = EOS_LOBBYSEARCH_FIND_API_LATEST;
	FindOptions.LocalUserId = *LocalUserId;

	EOS_LobbySearch_Find(*LobbySearchHandle, &FindOptions, this, OnFindLobbyComplete);
}

void LobbyManager::OnFindLobbyComplete(const EOS_LobbySearch_FindCallbackInfo* Data) {
	LobbyManager* Manager = static_cast<LobbyManager*>(Data->ClientData);
	if (Manager == nullptr) {
		std::cerr << "[LobbyManager] Critical error: ClientData was null in find lobby callback." << std::endl;
		return;
	}
	if (Data->ResultCode == EOS_EResult::EOS_Success) {
		std::cout << "[LobbyManager] Callback: Lobby search successful." << std::endl;
		EOS_LobbySearch_GetSearchResultCountOptions CountOptions = {};
		CountOptions.ApiVersion = EOS_LOBBYSEARCH_GETSEARCHRESULTCOUNT_API_LATEST;
		uint32_t searchResultCount = EOS_LobbySearch_GetSearchResultCount(*Manager->LobbySearchHandle, &CountOptions);
		std::cout << "[LobbyManager] Found " << searchResultCount << " lobbies." << std::endl;
		if (searchResultCount > 0) {
			// For simplicity, just get the first lobby found
			EOS_LobbySearch_CopySearchResultByIndexOptions CopyOptions = {};
			CopyOptions.ApiVersion = EOS_LOBBYSEARCH_COPYSEARCHRESULTBYINDEX_API_LATEST;
			CopyOptions.LobbyIndex = 0;
			EOS_EResult copyResult = EOS_LobbySearch_CopySearchResultByIndex(*Manager->LobbySearchHandle, &CopyOptions, &Manager->LobbyDetailsHandle);
			if (copyResult == EOS_EResult::EOS_Success) {
				std::cout << "[LobbyManager] Successfully copied lobby details." << std::endl;
				Manager->JoinLobby();
				Manager->isBusy = false;
			}
			else {
				std::cerr << "[LobbyManager] Failed to copy lobby details: " << EOS_EResult_ToString(copyResult) << std::endl;
				Manager->isBusy = false;
				return;
			}
		}
	}
	else {
		std::cerr << "[LobbyManager] Callback: Lobby search failed: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
		Manager->isBusy = false;
	}
};

void LobbyManager::JoinLobby() {
	isBusy = true;
	EOS_Lobby_JoinLobbyOptions JoinOptions = {};
	JoinOptions.ApiVersion = EOS_LOBBY_JOINLOBBYBYID_API_LATEST;
	JoinOptions.bCrossplayOptOut = EOS_FALSE;
	JoinOptions.bPresenceEnabled = EOS_TRUE;
	JoinOptions.LobbyDetailsHandle = LobbyDetailsHandle;
	JoinOptions.LocalRTCOptions = nullptr;
	JoinOptions.LocalUserId = *LocalUserId;
	JoinOptions.RTCRoomJoinActionType = EOS_ELobbyRTCRoomJoinActionType::EOS_LRRJAT_AutomaticJoin;
	EOS_Lobby_JoinLobby(*LobbyHandle, &JoinOptions, this, OnJoinLobbyComplete);
}

void LobbyManager::OnJoinLobbyComplete(const EOS_Lobby_JoinLobbyCallbackInfo* Data) {
	LobbyManager* Manager = static_cast<LobbyManager*>(Data->ClientData);
	if (Manager == nullptr) {
		std::cerr << "[LobbyManager] Critical error: ClientData was null in join lobby callback." << std::endl;
		return;
	}
	if (Data->ResultCode == EOS_EResult::EOS_Success) {
		std::cout << "[LobbyManager] Successfully joined lobby with ID: " << Data->LobbyId << std::endl;
		Manager->SetLobbyId(const_cast<EOS_LobbyId*>(&Data->LobbyId));
		EOS_LobbyDetails_GetLobbyOwnerOptions OwnerOptions = {};
		OwnerOptions.ApiVersion = EOS_LOBBYDETAILS_GETLOBBYOWNER_API_LATEST;
		Manager->HostId = new EOS_ProductUserId(EOS_LobbyDetails_GetLobbyOwner(Manager->LobbyDetailsHandle, &OwnerOptions));
		std::cout << "[LobbyManager] Lobby owner ID: " << *Manager->HostId << std::endl;
	}
	else {
		std::cerr << "[LobbyManager] Failed to join lobby: " << EOS_EResult_ToString(Data->ResultCode) << std::endl;
	}
	Manager->isBusy = false;
}

void LobbyManager::RegisterMemberStatusChange() {
	EOS_Lobby_AddNotifyLobbyMemberStatusReceivedOptions NotifyOptions = {};
	NotifyOptions.ApiVersion = EOS_LOBBY_ADDNOTIFYLOBBYMEMBERSTATUSRECEIVED_API_LATEST;
	MemberStatusChangeNotificationId = new EOS_NotificationId(EOS_Lobby_AddNotifyLobbyMemberStatusReceived(*LobbyHandle, &NotifyOptions, this, OnMemberStatusChange)
);
}

void LobbyManager::OnMemberStatusChange(const EOS_Lobby_LobbyMemberStatusReceivedCallbackInfo* Data) {
	LobbyManager* Manager = static_cast<LobbyManager*>(Data->ClientData);
	if (Manager == nullptr) {
		std::cerr << "[LobbyManager] Critical error: ClientData was null in member2 status change callback." << std::endl;
		return;
	}
	std::cout << "[LobbyManager] Member status changed in lobby " << Data->LobbyId
		<< " for user " << Data->TargetUserId
		<< " with status " << static_cast<int>(Data->CurrentStatus) << std::endl;
	Manager->ExternalUsers.push_back(const_cast<EOS_ProductUserId*>(&Data->TargetUserId));
	Manager->P2PConnections.push_back(new P2PManager(Manager->P2PHandle, Manager->LocalUserId, const_cast<EOS_ProductUserId*>(&Data->TargetUserId)));
}