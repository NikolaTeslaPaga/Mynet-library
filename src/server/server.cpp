#include "server.h"
namespace mynet {

Server::Server(const Config& config)
	:mConfig(config) { }
Server::~Server() {
	Stop();
}

Server::Server(Server&& other) noexcept
    :   running(other.running),
        mConfig(other.mConfig),
        mHost(other.mHost),
        mPeers(std::move(other.mPeers)),
        mPeerToID(std::move(other.mPeerToID)),
        mID(other.mID),
        onConnect(std::move(other.onConnect)),
        onDisconnect(std::move(other.onDisconnect)),
        onReceive(std::move(other.onReceive)) {
    other.mHost = nullptr;
}
Server& Server::operator=(Server&& other) noexcept {
    if (this != &other) {
        Stop();

        running = other.running;
        mConfig = other.mConfig;
        mHost = other.mHost;
        mPeers = std::move(other.mPeers);
        mPeerToID = std::move(other.mPeerToID);
        mID = other.mID;
        
        onConnect = std::move(other.onConnect);
        onDisconnect = std::move(other.onDisconnect);
        onReceive = std::move(other.onReceive);
        
        other.mHost = nullptr;
    }
    return *this;
}


bool Server::Start() {
    if (running)
        return false;

    mPeers.clear();
    mPeerToID.clear();


    ENetAddress address{};
    address.host = ENET_HOST_ANY;
    address.port = mConfig.port;

    mHost = enet_host_create(
        &address,
        static_cast<size_t>(mConfig.maxClients),
        static_cast<size_t>(mConfig.channels),
        static_cast<uint32_t>(mConfig.incomingBandwidth),
        static_cast<uint32_t>(mConfig.outgoingBandwidth)
    );

    if (!mHost) {
        std::cerr << "Failed to create ENet server\n";
        return false;
    }

    running = true;
    mPeers.reserve(mConfig.maxClients);
    return true;
}
bool Server::Stop() {
    if (!running)
        return false;
    running = false;
    DisconnectAll();

    std::this_thread::sleep_for(
        std::chrono::milliseconds(100)
    );

    if (mHost) {
        enet_host_destroy(mHost);
        mHost = nullptr;
    }
    mPeers.clear();
}


bool Server::Poll(uint32_t timeoutMs) {
    if (!running)
        return false;

    assert(mHost);

    ENetEvent event;
    while (enet_host_service(mHost, &event, timeoutMs) > 0)
        HandleEvent(event);
    return true;
}




bool Server::SendTo(ID id, const void* data, size_t size, uint8_t channel, bool reliable) {
    if (!mPeers.contains(id))
        return false;
    ENetPacket* packet = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    return enet_peer_send(GetPeer(id), channel, packet) == 0;
}
void Server::Broadcast(const void* data, size_t size, uint8_t channel, bool reliable) {
    ENetPacket* packet = enet_packet_create(data, size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_host_broadcast(mHost, channel, packet);
}


void Server::Disconnect(ID id, uint32_t data) {
    assert(mPeers.contains(id));
    enet_peer_disconnect(GetPeer(id), data);
}
void Server::DisconnectAll(uint32_t data) {
    for (const auto& [_, peer] : mPeers)
       enet_peer_disconnect(peer, data);
}


bool Server::ReConfig(const Config& config) {
    if (running)
        return false;

    mConfig = config;
    return true;
}



size_t Server::GetPeerAmount() const {
	return mPeers.size();
}
ENetPeer* Server::GetPeer(ID id) {
	assert(mPeers.contains(id));
	return mPeers.at(id);
}
const ENetPeer* Server::GetPeer(ID id) const {
	assert(mPeers.contains(id));
	return mPeers.at(id);
}
Server::ID Server::GetID(ENetPeer* peer) const {
    assert(mPeerToID.contains(peer));
	return mPeerToID.at(peer);
}
const Server::Config& Server::GetConfig() const {
    return mConfig;
}


void Server::HandleEvent(const ENetEvent& event) {
    switch (event.type) {
    case ENET_EVENT_TYPE_CONNECT: {
        AddPeer(event.peer);
        if (onConnect)
            onConnect(GetID(event.peer));

        break;
    }

    case ENET_EVENT_TYPE_DISCONNECT: {
        ID id = GetID(event.peer);

        if (onDisconnect)
            onDisconnect(id);

        DeletePeer(id);
        break;        
    }

    case ENET_EVENT_TYPE_RECEIVE: {

        if (onReceive)
            onReceive(PacketView(event));

        enet_packet_destroy(event.packet);
        break;
    }

    default:
        break;
    }
}


void Server::AddPeer(ENetPeer* peer) {
    assert(!mPeerToID.contains(peer));

    ID id = mID++;

    mPeers.insert(std::pair(id, peer));
    mPeerToID.insert(std::pair(peer, id));
}
void Server::DeletePeer(ENetPeer* peer) {
    assert(mPeerToID.contains(peer));
    mPeers.erase(mPeerToID.at(peer));
    mPeerToID.erase(peer);
}
void Server::DeletePeer(ID id) {
    assert(mPeers.contains(id));
    mPeerToID.erase(mPeers.at(id));
    mPeers.erase(id);
}


} //namespace mynet