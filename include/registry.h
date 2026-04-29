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


#ifndef LATTICE_NO_IMPLEMENTATION

namespace Lattice {

    // ==================== ENTITY ====================

    inline EntityID Registry::createEntity() {
        EntityID id = _nextID++;
        _alive.insert(id);
        return id;
    }

    inline void Registry::destroyEntity(EntityID id) {
        _alive.erase(id);

        for (auto& [type, storage] : _components) {
            storage->destroy(id);
        }
    }

    inline bool Registry::alive(EntityID id) const {
        return _alive.find(id) != _alive.end();
    }

    inline const std::unordered_set<EntityID>& Registry::entities() const {
        return _alive;
    }

    // ==================== STORAGE ACCESS ====================

    template<typename T>
    Storage<T>& Registry::getStorage() {
        auto type = std::type_index(typeid(T));

        auto it = _components.find(type);
        if (it == _components.end()) {
            auto storage = std::make_unique<Storage<T>>();
            auto* ptr = storage.get();
            _components[type] = std::move(storage);
            return *ptr;
        }

        return *static_cast<Storage<T>*>(it->second.get());
    }

    template<typename T>
    const Storage<T>& Registry::getStorage() const {
        auto it = _components.find(std::type_index(typeid(T)));
        if (it == _components.end())
            throw std::runtime_error("Storage does not exist");

        return *static_cast<Storage<T>*>(it->second.get());
    }

    // ==================== COMPONENT API ====================

    template<typename T>
    T& Registry::addComponent(EntityID id, T component) {
        auto& store = getStorage<T>().data;
        store[id] = std::move(component);
        return store.at(id);
    }

    template<typename T>
    T& Registry::getComponent(EntityID id) {
        auto& store = getStorage<T>().data;

        auto it = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Missing component");

        return it->second;
    }

    template<typename T>
    const T& Registry::getComponent(EntityID id) const {
        const auto& store = getStorage<T>().data;

        auto it = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Missing component");

        return it->second;
    }

    template<typename T>
    T* Registry::tryGetComponent(EntityID id) {
        auto& store = getStorage<T>().data;

        auto it = store.find(id);
        return (it == store.end()) ? nullptr : &it->second;
    }

    template<typename T>
    const T* Registry::tryGetComponent(EntityID id) const {
        const auto& store = getStorage<T>().data;

        auto it = store.find(id);
        return (it == store.end()) ? nullptr : &it->second;
    }

    template<typename T>
    bool Registry::hasComponent(EntityID id) const {
        auto it = _components.find(std::type_index(typeid(T)));
        if (it == _components.end()) return false;

        const auto& store = static_cast<const Storage<T>*>(it->second.get())->data;
        return store.find(id) != store.end();
    }

    template<typename T>
    void Registry::removeComponent(EntityID id) {
        getStorage<T>().data.erase(id);
    }

    // ==================== ITERATION ====================

    template<typename... Ts, typename Fn>
    void Registry::each(Fn&& fn) {
        for (EntityID id : _alive) {
            if ((hasComponent<Ts>(id) && ...)) {
                fn(id, getComponent<Ts>(id)...);
            }
        }
    }

} // namespace Lattice

#endif

#endif //LATTICE_REGISTRY_H
