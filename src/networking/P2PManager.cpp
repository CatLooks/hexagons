#include "networking/P2PManager.hpp"

void P2PManager::OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
	if (auto Manager = static_cast<P2PManager*>(Data->ClientData)) {
		Manager->HandleIncomingConnectionRequest(Data);
	}
}

void P2PManager::HandleIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
	if (Data->LocalUserId != LocalUserId) {
		std::cerr << "[P2PManager] Incoming connection request for unknown user." << std::endl;
		return;
	}
	EOS_P2P_AcceptConnectionOptions AcceptOptions = {};
	AcceptOptions.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
	AcceptOptions.LocalUserId = LocalUserId;
	AcceptOptions.RemoteUserId = Data->RemoteUserId;
	AcceptOptions.SocketId = Data->SocketId;
	EOS_EResult acceptResult = EOS_P2P_AcceptConnection(P2PHandle, &AcceptOptions);
	if (acceptResult == EOS_EResult::EOS_Success) {
		std::cout << "[P2PManager] Accepted incoming connection request from user." << std::endl;
		while (ReceivePacket()) {}
	}
	else {
		std::cerr << "[P2PManager] Failed to accept incoming connection request: " << EOS_EResult_ToString(acceptResult) << std::endl;
	}
}

void P2PManager::SendString(const std::string& Message) {
	if (PeerId == nullptr) {
		std::cerr << "[P2PManager] Cannot send message, PeerId is not set." << std::endl;
		return;
	}
	EOS_P2P_SendPacketOptions SendOptions = {};
	SendOptions.ApiVersion = EOS_P2P_SENDPACKET_API_LATEST;
	SendOptions.LocalUserId = LocalUserId;
	SendOptions.RemoteUserId = PeerId;
	SendOptions.SocketId = SocketId.get();
	SendOptions.Channel = 0;
	SendOptions.DataLengthBytes = static_cast<uint32_t>(Message.size());
	SendOptions.Data = Message.c_str();
	SendOptions.bAllowDelayedDelivery = EOS_TRUE;
	SendOptions.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
	SendOptions.bDisableAutoAcceptConnection = EOS_FALSE;

	EOS_EResult r = EOS_P2P_SendPacket(P2PHandle, &SendOptions);
	if (r != EOS_EResult::EOS_Success) {
		std::cerr << "[P2PManager] SendPacket failed: " << EOS_EResult_ToString(r) << std::endl;
	}
	else {
		std::cout << "[P2PManager] Sent packet (" << Message.size() << " bytes)" << std::endl;
	}
}

bool P2PManager::ReceivePacket() {
	std::cout << "[P2PManager] ReceivePacket start (local=" << static_cast<const void*>(LocalUserId)
		<< ", peer=" << static_cast<const void*>(PeerId)
		<< ", socket=" << SocketId->SocketName << ")" << std::endl;

	EOS_P2P_ReceivePacketOptions ReceiveOptions = {};
	ReceiveOptions.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
	ReceiveOptions.LocalUserId = LocalUserId;

	// Prealokowany bufor danych (dopasowany do MaxDataSizeBytes)
	char buffer[1024] = {};
	ReceiveOptions.MaxDataSizeBytes = static_cast<uint32_t>(sizeof(buffer));

	uint8_t channel = 0;
	uint32_t bytesWritten = static_cast<uint32_t>(sizeof(buffer));

	EOS_EResult receiveResult = EOS_P2P_ReceivePacket(P2PHandle, &ReceiveOptions, &PeerId, SocketId.get(), &channel, static_cast<void*>(buffer), &bytesWritten);

	if (receiveResult == EOS_EResult::EOS_NotFound) {
		// Brak pakietów
		// Commented out to reduce log spam
		//std::cout << "[P2PManager] ReceivePacket: no packets available." << std::endl;
		return false;
	}
	if (receiveResult != EOS_EResult::EOS_Success) {
		std::cerr << "[P2PManager] Failed to receive packet: " << EOS_EResult_ToString(receiveResult) << std::endl;
		return false;
	}

	std::cout << "[P2PManager] ReceivePacket: received " << bytesWritten << " bytes on channel "
		<< static_cast<int>(channel) << " from peer " << static_cast<const void*>(PeerId) << std::endl;

	OnMessageReceived.invoke(buffer);
	return true;
}