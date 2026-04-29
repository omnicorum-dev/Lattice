//
// Created by Nico Russo on 4/27/26.
//

#ifndef LATTICE_REGISTRY_H
#define LATTICE_REGISTRY_H

#include <any>
#include <typeindex>
#include <unordered_set>
#include <glm/glm.hpp>

namespace Lattice {
    using EntityID = uint32_t;
    inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

    // ==================== STORAGE LAYER ====================

    struct IStorage {
        virtual void destroy(EntityID id) = 0;
        virtual ~IStorage() = default;
    };

    template<typename T>
    struct Storage : IStorage {
        std::unordered_map<EntityID, T> data;

        void destroy(EntityID id) override {
            data.erase(id);
        }
    };

    // ==================== REGISTRY ====================

    struct Registry {
        // -------- Entity management --------
        EntityID createEntity();
        void destroyEntity(EntityID id);
        [[nodiscard]] bool alive(EntityID id) const;

        [[nodiscard]] const std::unordered_set<EntityID>& entities() const;

        // -------- Components --------
        template<typename T>
        T& addComponent(EntityID id, T component);

        template<typename T>
        T& getComponent(EntityID id);

        template<typename T>
        const T& getComponent(EntityID id) const;

        template<typename T>
        T* tryGetComponent(EntityID id);

        template<typename T>
        const T* tryGetComponent(EntityID id) const;

        template<typename T>
        bool hasComponent(EntityID id) const;

        template<typename T>
        void removeComponent(EntityID id);

        // -------- Iteration --------
        template<typename... Ts, typename Fn>
        void each(Fn&& fn);

    private:
        EntityID _nextID = 0;
        std::unordered_set<EntityID> _alive;

        std::unordered_map<std::type_index, std::unique_ptr<IStorage>> _components;

        template<typename T>
        Storage<T>& getStorage();

        template<typename T>
        const Storage<T>& getStorage() const;
    };
}


namespace Lattice {
    template<typename T>
T& Registry::addComponent(EntityID id, T component) {
        auto& storage = getStorage<T>();
        storage.data[id] = std::move(component);
        return storage.data[id];
    }

    template<typename T>
    T& Registry::getComponent(EntityID id) {
        return getStorage<T>().data.at(id);
    }

    template<typename T>
    const T& Registry::getComponent(EntityID id) const {
        return getStorage<T>().data.at(id);
    }

    template<typename T>
    T* Registry::tryGetComponent(EntityID id) {
        auto& storage = getStorage<T>();
        auto it = storage.data.find(id);
        return it != storage.data.end() ? &it->second : nullptr;
    }

    template<typename T>
    const T* Registry::tryGetComponent(EntityID id) const {
        const auto& storage = getStorage<T>();
        auto it = storage.data.find(id);
        return it != storage.data.end() ? &it->second : nullptr;
    }

    template<typename T>
    bool Registry::hasComponent(EntityID id) const {
        auto it = _components.find(typeid(T));
        if (it == _components.end()) return false;
        const auto& storage = static_cast<const Storage<T>&>(*it->second);
        return storage.data.count(id);
    }

    template<typename T>
    void Registry::removeComponent(EntityID id) {
        getStorage<T>().data.erase(id);
    }

    template<typename... Ts, typename Fn>
    void Registry::each(Fn&& fn) {
        for (EntityID id : _alive) {
            if ((hasComponent<Ts>(id) && ...))
                fn(id, getComponent<Ts>(id)...);
        }
    }

    template<typename T>
    Storage<T>& Registry::getStorage() {
        auto& ptr = _components[typeid(T)];
        if (!ptr) ptr = std::make_unique<Storage<T>>();
        return static_cast<Storage<T>&>(*ptr);
    }

    template<typename T>
    const Storage<T>& Registry::getStorage() const {
        auto it = _components.find(typeid(T));
        if (it == _components.end())
            throw std::runtime_error("Storage not found");
        return static_cast<const Storage<T>&>(*it->second);
    }
}


#endif //LATTICE_REGISTRY_H
