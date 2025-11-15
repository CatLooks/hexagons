#pragma once
#include <iostream>
#include <eos_common.h>
#include <eos_p2p.h>
#include <string>
#include <cstring>
#include <memory>

class P2PManager {
public:
	P2PManager(EOS_HP2P Handle, EOS_ProductUserId user, EOS_ProductUserId peer) : P2PHandle(Handle), UserId(user), PeerId(peer) {
		SocketId->ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strncpy(SocketId->SocketName, "DefaultSocket", EOS_P2P_SOCKETID_SOCKETNAME_SIZE);
		
		EOS_P2P_AddNotifyPeerConnectionRequestOptions IncomingConnectionOptions = {};
		IncomingConnectionOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
		IncomingConnectionOptions.LocalUserId = UserId;
		EOS_P2P_AddNotifyPeerConnectionRequest(P2PHandle, &IncomingConnectionOptions, this, OnIncomingConnectionRequest);
	}

	void SetPeerId(EOS_ProductUserId id) { PeerId = id; };
	void SendString(const std::string& Message);
	void ReceivePacket();
private:
	static void EOS_CALL OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data);

	EOS_HP2P P2PHandle = nullptr;
	std::shared_ptr<EOS_P2P_SocketId> SocketId = std::make_shared<EOS_P2P_SocketId>();
	EOS_ProductUserId UserId = nullptr;
	EOS_ProductUserId PeerId = nullptr;
	bool isHost = false;
};