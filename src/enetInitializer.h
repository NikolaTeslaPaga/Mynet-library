#pragma once
#ifndef _ENET_INIT_H_
#define _ENET_INIT_H_

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

} //namespace mynet


#endif //_ENET_INIT_H