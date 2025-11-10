#include "eos/P2PManager.hpp"

void P2PManager::OnIncomingConnectionRequest(const EOS_P2P_OnIncomingConnectionRequestInfo* Data) {
	P2PManager* Manager = static_cast<P2PManager*>(Data->ClientData);
	if (Manager == nullptr) {
		std::cerr << "[P2PManager] Critical error: ClientData was null in incoming connection request callback." << std::endl;
		return;
	}
	if (Data->LocalUserId != *Manager->UserId) {
		std::cerr << "[P2PManager] Incoming connection request for unknown user." << std::endl;
		return;
	}
	EOS_P2P_AcceptConnectionOptions AcceptOptions = {};
	AcceptOptions.ApiVersion = EOS_P2P_ACCEPTCONNECTION_API_LATEST;
	AcceptOptions.LocalUserId = *Manager->UserId;
	AcceptOptions.RemoteUserId = Data->RemoteUserId;
	AcceptOptions.SocketId = Data->SocketId;
	EOS_EResult acceptResult = EOS_P2P_AcceptConnection(*Manager->P2PHandle, &AcceptOptions);
	if (acceptResult == EOS_EResult::EOS_Success) {
		std::cout << "[P2PManager] Accepted incoming connection request from user." << std::endl;
		Manager->SetPeerId(std::make_shared<EOS_ProductUserId>(Data->RemoteUserId));
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
	SendOptions.LocalUserId = *UserId;
	SendOptions.RemoteUserId = *PeerId;
	SendOptions.SocketId = SocketId;
	SendOptions.Channel = 0;
	SendOptions.DataLengthBytes = static_cast<uint32_t>(Message.size());
	SendOptions.Data = Message.c_str();
	SendOptions.bAllowDelayedDelivery = EOS_TRUE;
	SendOptions.Reliability = EOS_EPacketReliability::EOS_PR_ReliableOrdered;
	SendOptions.bDisableAutoAcceptConnection = EOS_FALSE;

	EOS_EResult r = EOS_P2P_SendPacket(*P2PHandle, &SendOptions);
	if (r != EOS_EResult::EOS_Success) {
		std::cerr << "[P2PManager] SendPacket failed: " << EOS_EResult_ToString(r) << std::endl;
	} else {
		std::cout << "[P2PManager] Sent packet (" << Message.size() << " bytes)" << std::endl;
	}
}

void P2PManager::ReceivePacket() {
	EOS_P2P_ReceivePacketOptions ReceiveOptions = {};
	ReceiveOptions.ApiVersion = EOS_P2P_RECEIVEPACKET_API_LATEST;
	ReceiveOptions.LocalUserId = *UserId;

	// Prealokowany bufor danych (dopasowany do MaxDataSizeBytes)
	char buffer[1024] = {};
	ReceiveOptions.MaxDataSizeBytes = static_cast<uint32_t>(sizeof(buffer));

	uint8_t channel = 0;
	uint32_t bytesWritten = static_cast<uint32_t>(sizeof(buffer));

	// Poprawne wywo³anie z buforem i rozmiarem
	EOS_EResult receiveResult = EOS_P2P_ReceivePacket(*P2PHandle, &ReceiveOptions, PeerId.get(), SocketId, &channel, static_cast<void*>(buffer), &bytesWritten);

	if (receiveResult == EOS_EResult::EOS_NotFound) {
		// Brak pakietów
		return;
	}
	if (receiveResult != EOS_EResult::EOS_Success) {
		std::cerr << "[P2PManager] Failed to receive packet: " << EOS_EResult_ToString(receiveResult) << std::endl;
		return;
	}

	// Zbuduj std::string z dok³adn¹ liczb¹ odebranych bajtów (bez wymagania null-terminatora)
	std::string message(buffer, buffer + static_cast<size_t>(bytesWritten));
	std::cout << "[P2PManager] Received packet (" << bytesWritten << " bytes): " << message << std::endl;

	if (!isHost) {
		// Echo back the message if not host
		SendString(message);
	}
}