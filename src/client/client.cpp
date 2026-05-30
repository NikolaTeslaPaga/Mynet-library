#include "client.h"


namespace mynet {

    Client::Client(const Config& config)
        :   mConfig(config) {}
    Client::~Client() {
        Stop();
    }

    Client::Client(Client&& other) noexcept
        :   started(other.started),
            connected(other.connected),
            mConfig(other.mConfig),
            mHost(other.mHost),
            mServerPeer(other.mServerPeer),
            onConnect(std::move(other.onConnect)),
            onDisconnect(std::move(other.onDisconnect)),
            onReceive(std::move(other.onReceive)) {
        other.mHost = nullptr;
        other.mServerPeer = nullptr;
    }
    Client& Client::operator=(Client&& other) noexcept {
        if (this != &other) {
            Stop();

            started = other.started;
            connected = other.connected;
            mConfig = other.mConfig;
            mHost = other.mHost;
            mServerPeer = other.mServerPeer;

            onConnect = std::move(other.onConnect);
            onDisconnect = std::move(other.onDisconnect);
            onReceive = std::move(other.onReceive);

            other.mHost = nullptr;
            other.mServerPeer = nullptr;
        }
        return *this;
    }

    bool Client::Start() {
        if (started)
            return false;
        mHost = enet_host_create(
            nullptr,
            1,
            static_cast<size_t>(mConfig.channels),
            static_cast<uint32_t>(mConfig.incomingBandwidth),
            static_cast<uint32_t>(mConfig.outgoingBandwidth)
        );

        if (!mHost) {
            std::cerr << "Failed to create ENet client\n";
            return false;
        }
        started = true;
        return true;
    }
    bool Client::Stop() {
        if (!started)
            return false;

        if (connected)
            Disconnect();

        if (mHost) {
            enet_host_destroy(mHost);
            mHost = nullptr;
        }

        mServerPeer = nullptr;
        started = false;
        return true;
    }

    bool Client::Connect(const std::string& host, uint16_t port, uint32_t timeoutMs) {
        if (!started || connected)
            return false;

        assert(mHost);

        ENetAddress address{};
        enet_address_set_host(&address, host.c_str());
        address.port = port;

        mServerPeer = enet_host_connect(mHost, &address, mConfig.channels, 0);

        if (!mServerPeer) {
            std::cerr << "No available peers for connection\n";
            return false;
        }

        ENetEvent event;
        if (enet_host_service(mHost, &event, timeoutMs) > 0 &&
            event.type == ENET_EVENT_TYPE_CONNECT) {

            if (onConnect)
                onConnect();

            connected = true;

            return true;
        }

        enet_peer_reset(mServerPeer);
        mServerPeer = nullptr;
        return false;
    }
    void Client::Disconnect(uint32_t data) {
        if (!mServerPeer)
            return;

        enet_peer_disconnect(mServerPeer, data);

        ENetEvent event;

        while (enet_host_service(mHost, &event, 100) > 0)
            if (event.type == ENET_EVENT_TYPE_DISCONNECT) {
                mServerPeer = nullptr;
                connected = false;
                return;
            }

        enet_peer_reset(mServerPeer);

        mServerPeer = nullptr;
        connected = false;
    }
    bool Client::IsConnected() const {
        return connected && mServerPeer && mServerPeer->state == ENET_PEER_STATE_CONNECTED;
    }

    bool Client::Poll(uint32_t timeoutMs) {
        if (!connected)
            return false;
        assert(mHost);

        ENetEvent event;
        while (enet_host_service(mHost, &event, timeoutMs) > 0)
            HandleEvent(event);
        return true;
    }
    bool Client::Send(const void* data, size_t size, uint8_t channel, bool reliable) {
        if (!connected)
            return false;

        ENetPacket* packet = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);

        return enet_peer_send(mServerPeer, channel, packet) == 0;
    }

    bool Client::ReConfig(const Config& config) {
        if (connected)
            return false;
        mConfig = config;
        return true;
    }

    void Client::HandleEvent(const ENetEvent& event) {
        switch (event.type) {

        case ENET_EVENT_TYPE_RECEIVE: {
            if (onReceive)
                onReceive(PacketView(event));

            enet_packet_destroy(event.packet);

            break;
        }

        case ENET_EVENT_TYPE_DISCONNECT: {
            mServerPeer = nullptr;

            if (onDisconnect)
                onDisconnect();

            break;
        }

        default:
            break;
        }
    }

} // namespace mynet