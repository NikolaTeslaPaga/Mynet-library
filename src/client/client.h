#pragma once
#ifndef _MYNET_CLIENT_H_
#define _MYNET_CLIENT_H_

#include <enet/enet.h>

#include <iostream>
#include <cassert>
#include <cstdint>
#include <functional>
#include <string>

namespace mynet {

    class Client {
    public:

#pragma region Types

        struct Config {
            size_t channels = 2;

            size_t incomingBandwidth = 0;
            size_t outgoingBandwidth = 0;

            size_t packetSize = 1024;
            size_t bufferSize = 64 * 1024;
        };

        struct PacketView {
            uint8_t channel;
            const uint8_t* data;
            size_t size;

            PacketView(const ENetEvent& event)
                : channel(event.channelID),
                data(event.packet->data),
                size(event.packet->dataLength) {}
        };

#pragma endregion

#pragma region Lifecycle

        Client() = default;
        Client(const Config& config);
        ~Client();

        Client(const Client&) = delete;
        Client& operator=(const Client&) = delete;

        Client(Client&&) noexcept;
        Client& operator=(Client&&) noexcept;

        bool Start();
        bool Stop();

#pragma endregion

#pragma region Connection

        bool Connect(const std::string& host, uint16_t port, uint32_t timeoutMs = 5000);
        void Disconnect(uint32_t data = 0);

        bool IsConnected() const;

#pragma endregion

#pragma region Main Loop

        bool Poll(uint32_t timeoutMs = 0);

#pragma endregion

#pragma region Sending

        bool Send(const void* data, size_t size, uint8_t channel = 0, bool reliable = true);

#pragma endregion

#pragma region ReConfig

        bool ReConfig(const Config& config);

#pragma endregion

#pragma region Callbacks

        std::function<void()> onConnect = [&]() {
            std::cout << "Cliented connected to server!\n";
        };
        std::function<void()> onDisconnect = [&]() {
            std::cout << "Cliented disconnected from server!\n";
        };
        std::function<void(const PacketView&)> onReceive = [&](const PacketView& packet) {
            std::cout << "Packet received from server!\n";
            std::cout << std::string((char*)packet.data, packet.size) << '\n';
        };

#pragma endregion

#pragma region Internal helpers
    private:
        void HandleEvent(const ENetEvent& event);

#pragma endregion

    private:

        bool started = false;
        bool connected = false;
        Config mConfig;

        ENetHost* mHost = nullptr;
        ENetPeer* mServerPeer = nullptr;
    };

} // namespace mynet

#endif // _CLIENT_