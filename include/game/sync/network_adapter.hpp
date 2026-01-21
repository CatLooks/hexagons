#pragma once

#include "game/sync/adapter.hpp"
#include "networking/Net.hpp"
#include "game/serialize/messages.hpp" 
#include "game/serialize/moves.hpp"    
#include <queue>
#include <cassert>

class NetworkAdapter : public Adapter {
private:
    Net& _net;
    
    // Queues to store data received from the network until the GameState asks for it
    std::queue<Packet<Messages::Event>> _eventQueue;
    std::queue<Packet<History::UniqList>> _moveListQueue;

    // Protocol headers to distinguish between Events and Move Lists
    enum PacketType : uint8_t {
        Type_Event = 0,
        Type_MoveList = 1
    };

public:
    NetworkAdapter(Net& net, uint32_t localPlayerId) : _net(net) {
        this->id = localPlayerId;

        // Bind to the Net system's packet receiver
        _net.OnPacketReceived.add([this](const std::string& sender, sf::Packet& packet) {
            this->onPacketInternal(sender, packet);
        });
    }

    // --- Sending Data (Game -> Network) ---

    // 1. Send an Event (Chat, Next Turn, Init, etc.)
    void send(Packet<Messages::Event> evt) override {
        sf::Packet packet;
        
        // Write Header
        packet << (uint8_t)Type_Event;
        packet << evt.id; // Write the Player ID who generated this event

        // Write the actual message using YOUR serialization code
        Serialize::encodeMessage(packet, evt.value);

        _net.send(packet);
    }

    // 2. Send a List of Moves (Unit attacks, movements, etc.)
    void send_list(Packet<History::SpanList> list) override {
        sf::Packet packet;

        // Write Header
        packet << (uint8_t)Type_MoveList;
        packet << list.id;

        // Write how many moves are in the list
        uint32_t count = (uint32_t)list.value.size();
        packet << count;

        // Write each move using existing move serializer
        for (const auto& move : list.value) {
            Serialize::encodeMove(packet, move.get());
        }

        _net.send(packet);
    }

    // --- Receiving Data (Network -> Game) ---

    OptPacket<Messages::Event> recv() override {
        if (_eventQueue.empty()) return std::nullopt;
        
        auto val = std::move(_eventQueue.front());
        _eventQueue.pop();
        return val;
    }

    OptPacket<History::UniqList> recv_list() override {
        if (_moveListQueue.empty()) return std::nullopt;

        auto val = std::move(_moveListQueue.front());
        _moveListQueue.pop();
        return val;
    }

private:
    // Called when Net receives bytes
    void onPacketInternal(const std::string& sender, sf::Packet& packet) {
        uint8_t type;
        
        // REMOVED: assert(packet.endOfPacket()); -- wrong place, packet is full here
        
        if (!(packet >> type)) return; // Safety check

        uint32_t playerId;
        if (!(packet >> playerId)) return;

        if (type == Type_Event) {
            auto msg = Serialize::decodeMessage(packet);
            if (msg) {
                _eventQueue.push({ std::move(*msg), playerId });
            }
        }
        else if (type == Type_MoveList) {
            uint32_t count;
            if (packet >> count) {
                History::UniqList moves;
                moves.reserve(count);
                
                for (uint32_t i = 0; i < count; ++i) {
                    auto move = Serialize::decodeMove(packet);
                    if (move) moves.push_back(std::move(move));
                }
                
                if (!moves.empty()) {
                    _moveListQueue.push({ std::move(moves), playerId });
                }
            }
        }

        // CORRECT PLACE: Check that we consumed exactly what was sent.
        // If this triggers, either the sender wrote too much or we read too little.
        assert(packet.endOfPacket());
    }
};