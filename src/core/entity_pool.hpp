#pragma once

#include <cstdint>
#include <vector>

const uint32_t INVALID_INDEX = INT32_MAX;
const uint32_t INVALID_GENERATION = INT32_MAX;
struct EntityHandle {
    size_t index = INVALID_INDEX;
    uint32_t generation = INVALID_GENERATION;

    bool IsValid() const { return index != INVALID_INDEX && generation != INVALID_GENERATION; }
};

template <typename T> struct Slot {
    T ref;
    uint32_t generation;
    bool alive;
    EntityHandle handle = {};
};

template <typename T> struct EntityPool {
    std::vector<Slot<T>> data;
    std::vector<uint32_t> free_indices;

    void clear() {
        data.clear();
        free_indices.clear();
    }
};

template <typename T> EntityHandle CreateEntity(EntityPool<T>& pool, T entity) {
    EntityHandle handle = {};
    if (!pool.free_indices.empty()) {
        handle.index = pool.free_indices.back();
        pool.free_indices.pop_back();

        Slot<T>& slot = pool.data[handle.index];
        slot.ref = std::move(entity);
        slot.alive = true;
        handle.generation = slot.generation;
        slot.handle = handle;
    } else {
        handle.index = pool.data.size();
        handle.generation = 0;
        pool.data.push_back(
            {.ref = std::move(entity), .generation = handle.generation, .alive = true, .handle = handle});
    }

    return handle;
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
