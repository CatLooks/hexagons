#pragma once
#include <iostream>
#include <eos_common.h>
#include <eos_p2p.h>
#include <string>
#include <cstring>
#include <memory>
#include "templated/delegate.hpp"

class P2PManager {
public:
	P2PManager(EOS_HP2P Handle, EOS_ProductUserId user, EOS_ProductUserId peer) : P2PHandle(Handle), LocalUserId(user), PeerId(peer) {
		SocketId->ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strncpy(SocketId->SocketName, "DefaultSocket", EOS_P2P_SOCKETID_SOCKETNAME_SIZE);

		EOS_P2P_AddNotifyPeerConnectionRequestOptions IncomingConnectionOptions = {};
		IncomingConnectionOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
		IncomingConnectionOptions.LocalUserId = LocalUserId;
		EOS_P2P_AddNotifyPeerConnectionRequest(P2PHandle, &IncomingConnectionOptions, this, OnIncomingConnectionRequest);
	}

	Delegate<void(const std::string&)> OnMessageReceived;

	EOS_ProductUserId GetPeerId() const { return PeerId; };
	
	template<typename T> //Temporary template function definition
	void SendPacket(const T* Data, uint32_t DataSize);
	
	bool ReceivePacket();

private:
	void HandleIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data);
	static void EOS_CALL OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data);

	EOS_HP2P P2PHandle = nullptr;
	std::shared_ptr<EOS_P2P_SocketId> SocketId = std::make_shared<EOS_P2P_SocketId>();
	EOS_ProductUserId LocalUserId = nullptr;
	EOS_ProductUserId PeerId = nullptr;
	bool isHost = false;
};