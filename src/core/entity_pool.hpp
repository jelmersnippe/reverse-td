#pragma once

#include <cstdint>
#include <limits>
#include <vector>

const uint32_t INVALID_INDEX = std::numeric_limits<uint32_t>::max();
struct EntityHandle {
    size_t index = INVALID_INDEX;
    uint32_t generation = INVALID_INDEX;

    bool IsValid() const { return index != INVALID_INDEX && generation != INVALID_INDEX; }
};

template <typename T> struct Slot {
    T ref;
    uint32_t generation;
    bool alive;
};

template <typename T> struct EntityPool {
    std::vector<Slot<T>> data;
    std::vector<uint32_t> free_indices;
};

template <typename T> EntityHandle CreateEntity(EntityPool<T>& pool, T entity) {
    uint32_t index = INVALID_INDEX;
    uint32_t generation = INVALID_INDEX;
    if (!pool.free_indices.empty()) {
        index = pool.free_indices.back();
        pool.free_indices.pop_back();

        Slot<T>& slot = pool.data[index];
        slot.ref = entity;
        slot.alive = true;
        generation = slot.generation;
    } else {
        index = pool.data.size();
        generation = 0;
        pool.data.push_back(Slot<T>{.ref = entity, .generation = 0, .alive = true});
    }

    return {.index = index, .generation = generation};
}

template <typename T> void DestroyEntity(EntityPool<T>& pool, EntityHandle handle) {
    if (!IsValidEntity(pool, handle)) return;

    pool.data[handle.index].generation += 1;
    pool.data[handle.index].alive = false;
    pool.free_indices.push_back(handle.index);
}

template <typename T> T* GetEntity(EntityPool<T>& pool, EntityHandle handle) {
    if (!handle.IsValid() || !IsValidEntity(pool, handle)) return nullptr;

    return &pool.data[handle.index].ref;
}

template <typename T> bool IsValidEntity(const EntityPool<T>& pool, EntityHandle handle) {
    return handle.index < pool.data.size() && pool.data[handle.index].generation == handle.generation;
}
