#pragma once
#ifndef _ENET_GLOBAL_H_
#define _ENET_GLOBAL_H_

#include <enet/enet.h>
#include <stdexcept>
#include <atomic>

namespace mynet {

class ENetInitializer {
public:
    ENetInitializer() {
        if (refCount.fetch_add(1, std::memory_order_acq_rel) == 0)
            if (enet_initialize() != 0)
                throw std::runtime_error("Failed to initialize ENet");
    }

    ~ENetInitializer() {
        if (refCount.fetch_sub(1, std::memory_order_acq_rel) == 1)
            enet_deinitialize();
    }

    ENetInitializer(const ENetInitializer&) = delete;
    ENetInitializer& operator=(const ENetInitializer&) = delete;

    ENetInitializer(ENetInitializer&&) = delete;
    ENetInitializer& operator=(ENetInitializer&&) = delete;

private:
    static inline std::atomic<size_t> refCount{ 0 };
}; //class ENetInitializer

std::string GetLocalIP() {
    char hostName[256];

    if (gethostname(hostName, sizeof(hostName)) != 0)
        return "127.0.0.1";

    ENetAddress addr;
    if (enet_address_set_host(&addr, hostName) != 0)
        return "127.0.0.1";

    char ip[64];
    enet_address_get_host_ip(&addr, ip, sizeof(hostName));
    return ip;
}



} //namespace mynet


#endif //_ENET_GLOBAL_H