#pragma once
#include <iostream>
#include <eos_common.h>
#include <eos_p2p.h>
#include <string>
#include <cstring>
#include <memory>

class P2PManager {
public:
	P2PManager(EOS_HP2P* Handle, EOS_ProductUserId* Id) : P2PHandle(Handle), UserId(Id) {
		SocketId->ApiVersion = EOS_P2P_SOCKETID_API_LATEST;
		strncpy(SocketId->SocketName, "DefaultSocket", EOS_P2P_SOCKETID_SOCKETNAME_SIZE);
		
		EOS_P2P_AddNotifyPeerConnectionRequestOptions IncomingConnectionOptions = {};
		IncomingConnectionOptions.ApiVersion = EOS_P2P_ADDNOTIFYPEERCONNECTIONREQUEST_API_LATEST;
		IncomingConnectionOptions.LocalUserId = *UserId;
		EOS_P2P_AddNotifyPeerConnectionRequest(*P2PHandle, &IncomingConnectionOptions, this, OnIncomingConnectionRequest);
	}

	void SendString(const std::string& Message);
	void SetPeerId(std::shared_ptr<EOS_ProductUserId> Id) { PeerId = Id; }
	void ReceivePacket();
	void setHost(bool Host) { isHost = Host; };
private:
	static void EOS_CALL OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data);

	EOS_HP2P* P2PHandle = nullptr;
	EOS_P2P_SocketId* SocketId = new EOS_P2P_SocketId();
	std::shared_ptr<EOS_ProductUserId> UserId = nullptr;
	std::shared_ptr<EOS_ProductUserId> PeerId = nullptr;
	bool isHost = false;
};