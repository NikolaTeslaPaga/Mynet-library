#pragma once
#ifndef _MYNET_SERVER_H_
#define _MYNET_SERVER_H_

#include <enet/enet.h>

#include <cassert>
#include <iostream>
#include <unordered_map>
#include <functional>
#include <format>
#include <string_view>
#include <cstdint>

namespace mynet {

class Server {
public:

#pragma region Types
	// ------------------------------
	// TYPES
	// ------------------------------

	using ID = uint32_t;

	struct Config {
		uint16_t port = 1234;
		size_t maxClients = 32;

		size_t channels = 2;

		size_t incomingBandwidth = 0;
		size_t outgoingBandwidth = 0;

		size_t packetSize = 1024;
		size_t bufferSize = 64 * 1024;
	};

	struct PacketView {
		ENetPeer* sender;
		uint8_t channel;
		const uint8_t* data;
		size_t size;

		PacketView(const ENetEvent& event)
			: sender(event.peer), channel(event.channelID), data(event.packet->data), size(event.packet->dataLength) {}
	};

#pragma endregion

#pragma region Lifecycle
	// ------------------------------
	// LIFECYCLE
	// ------------------------------

	Server() = default;
	Server(const Config& config);
	~Server();

	Server(Server&& other) noexcept;
	Server& operator=(Server&& other) noexcept;

	Server(const Server&) = delete;
	Server& operator=(const Server&) = delete;


	bool Start();
	bool Stop();

#pragma endregion

#pragma region Main Loop
	// ------------------------------
	// MAIN LOOP
	// ------------------------------

	bool Poll(uint32_t timeoutMs = 0);

#pragma endregion

#pragma region Sending
	// ------------------------------
	// SENDING
	// ------------------------------

	bool SendTo(ID slaveID, const void* data, size_t size, uint8_t channel = 0, bool reliable = true);

	void Broadcast(const void* data, size_t size, uint8_t channel = 0, bool reliable = true);

#pragma endregion

#pragma region Peer Management
	// ------------------------------
	// PEER MANAGEMENT
	// ------------------------------

	void Disconnect(ID id, uint32_t data = 0);
	void DisconnectAll(uint32_t data = 0);

#pragma endregion

#pragma region Callbacks
	// ------------------------------
	// CALLBACKS
	// ------------------------------

	std::function<void(ID)> onConnect = [&](ID id) {
		std::cout << "Client connected: " << id << '\n';

		const char* msg = "Welcome!";
		SendTo(id, msg, strlen(msg) + 1);

		std::string broadcast = std::format("Say hello to nr {}", id);
		Broadcast(broadcast.data(), broadcast.size() + 1);
	};
	std::function<void(ID)> onDisconnect = [&](ID id) {
		std::cout << "Client disconnected: " << id << '\n';

		std::string broadcast = std::format("Say goodbye to nr {}", id);
		Broadcast(broadcast.data(), broadcast.size() + 1);
	};
	std::function<void(const PacketView&)> onReceive =
		[&](const PacketView& packet) {

		std::string msg(
			reinterpret_cast<const char*>(packet.data),
			packet.size > 0 ? packet.size - 1 : 0
		);

		auto id = GetID(packet.sender);
		std::cout << "Recieved from " << id << ":\n";
		std::cout << msg << '\n';

		const char* confirmation = "Message Received!";
		SendTo(id, confirmation, strlen(confirmation) + 1);

		std::string broadcast = std::format("Received from {}: {}", id, msg);
		Broadcast(broadcast.data(), broadcast.size() + 1);
	};

#pragma endregion

#pragma region ReConfig

	bool ReConfig(const Config& config);

#pragma endregion

#pragma region Get funcs
	// ------------------------------
	// GET FUNCS
	// ------------------------------

	size_t GetPeerAmount() const;

	ENetPeer* GetPeer(ID id);
	const ENetPeer* GetPeer(ID id) const;
	ID GetID(ENetPeer* peer) const;

	const Config& GetConfig() const;
#pragma endregion

#pragma region Internal Helpers
	// ------------------------------
	// INTERNAL HELPERS
	// ------------------------------

private:

	void HandleEvent(const ENetEvent& event);

	void AddPeer(ENetPeer* peer);
	void DeletePeer(ENetPeer* peer);
	void DeletePeer(ID id);

#pragma endregion

#pragma region Member variables
private:

	bool running = false;

	Config mConfig;

	ENetHost* mHost = nullptr;
	std::unordered_map<ID, ENetPeer*> mPeers;
	std::unordered_map<ENetPeer*, ID> mPeerToID;

	ID mID = 0;

#pragma endregion


}; //class Server

} //namespace mynet


#endif // _SERVER_