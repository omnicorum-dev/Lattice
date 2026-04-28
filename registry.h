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






















#if 0
namespace Lattice {
    // ==================== ENTITY / REGISTRY ====================

    using EntityID = glm::u32;
    inline constexpr EntityID NULL_ENTITY = std::numeric_limits<EntityID>::max();

    /**
     * @brief A map-based entity-component registry.
     *
     * Entities are lightweight IDs. Components are stored in per-type
     * unordered_maps keyed by EntityID, so any component type can be
     * attached to any entity at any time without predeclaring archetypes.
     *
     * ### Typical usage
     * @code
     * Lattice::Registry registry;
     *
     * EntityID e = registry.createEntity();
     * registry.addComponent(e, Transform{});
     * registry.addComponent(e, Model3D{});
     *
     * registry.each<Transform, Model3D>([](EntityID id, Transform& t, Model3D& m) {
     *     // process entities that have both components
     * });
     * @endcode
     */
    struct Registry {
        /**
         * @brief Create a new entity and return its ID.
         * @return Unique EntityID for the new entity.
         */
        [[nodiscard]] EntityID createEntity();

        /**
         * @brief Destroy an entity and remove all its components.
         * @param id Entity to destroy.
         */
        void destroyEntity(EntityID id);

        /**
         * @brief Check whether an entity ID is currently alive.
         * @param id Entity to check.
         * @return `true` if the entity exists.
         */
        [[nodiscard]] bool alive(EntityID id) const;

        /**
         * @brief Add a component to an entity, replacing any existing component of the same type.
         * @tparam T         Component type.
         * @param id         Target entity.
         * @param component  Component value to store.
         * @return Reference to the stored component.
         */
        template<typename T>
        T& addComponent(EntityID id, T component);

        /**
         * @brief Get a reference to a component on an entity.
         *
         * Throws std::runtime_error if the entity does not have this component.
         * Use tryGetComponent() if presence is not guaranteed.
         *
         * @tparam T Component type.
         * @param id Target entity.
         * @return Reference to the component.
         */
        template<typename T>
        [[nodiscard]] T& getComponent(EntityID id);

        /// @overload Const version.
        template<typename T>
        [[nodiscard]] const T& getComponent(EntityID id) const;

        /**
         * @brief Get a pointer to a component, or nullptr if absent.
         * @tparam T Component type.
         * @param id Target entity.
         * @return Pointer to the component, or nullptr.
         */
        template<typename T>
        [[nodiscard]] T* tryGetComponent(EntityID id);

        /// @overload Const version.
        template<typename T>
        [[nodiscard]] const T* tryGetComponent(EntityID id) const;

        /**
         * @brief Check whether an entity has a specific component type.
         * @tparam T Component type.
         * @param id Target entity.
         * @return `true` if the component is present.
         */
        template<typename T>
        [[nodiscard]] bool hasComponent(EntityID id) const;

        /**
         * @brief Remove a component from an entity. No-op if absent.
         * @tparam T Component type.
         * @param id Target entity.
         */
        template<typename T>
        void removeComponent(EntityID id);

        /**
         * @brief Iterate all entities that have every listed component type.
         *
         * The callback receives the EntityID and a reference to each requested
         * component. Entities missing any of the listed types are skipped.
         *
         * @code
         * registry.each<Transform, Model3D>([](EntityID id, Transform& t, Model3D& m) {
         *     t.position.y += 1.f;
         * });
         * @endcode
         *
         * @tparam Ts        Component types to require.
         * @param  fn        Callback invoked for each matching entity.
         */
        template<typename... Ts>
        void each(std::function<void(EntityID, Ts&...)> fn);

        template<typename... Ts, typename Fn>
        void each(Fn&& fn);

        /**
         * @brief Return all living entity IDs.
         * @return Const reference to the set of active entity IDs.
         */
        [[nodiscard]] const std::unordered_set<EntityID>& entities() const;

    private:
        EntityID                                                     _nextID  = 0;
        std::unordered_set<EntityID>                                 _alive;
        /*
        std::unordered_map<std::type_index,
            std::unordered_map<EntityID, std::any>>                  _components;
            */

        std::unordered_map<std::type_index, std::any> _components;

        /*
        template<typename T>
        std::unordered_map<EntityID, std::any>& _store();

        template<typename T>
        const std::unordered_map<EntityID, std::any>& _store() const;
        */

        template<typename T>
        std::unordered_map<EntityID, T>& _store();

        template<typename T>
        const std::unordered_map<EntityID, T>& _store() const;
    };

    struct Prefab {
        std::function<EntityID(Registry&)> spawn;
    };

    /* EXAMPLE PREFAB
    Prefab playerPrefab = {
        [](Registry& reg)
        {
            EntityID e = reg.createEntity();

            reg.addComponent(e, Behaviours{});
            auto& behaviours = reg.getComponent<Behaviours>(e);

            behaviours.list.push_back(std::make_unique<PlayerController>());
            behaviours.list.push_back(std::make_unique<GunController>());
            behaviours.list.push_back(std::make_unique<HealthRegen>());

            return e;
        }
    }
    */

    /* RUNTIME UPDATE SYSTEM
    registry.each<Behaviours>(
    [&](EntityID id, Behaviours& behaviour)
    {
        for (auto& s : behaviour.list)
        {
            if (!behaviour->started)
            {
                s->entity = id;
                s->Start();
                s->started = true;
            }
            s->Update(dt);
        }
    });
    */
}
#endif

#if 0
#ifndef LATTICE_NO_IMPLEMENTATION
namespace Lattice {
    // ==================== REGISTRY ====================

    inline EntityID Registry::createEntity() {
        const EntityID id = _nextID++;
        _alive.insert(id);
        return id;
    }

    inline void Registry::destroyEntity(const EntityID id) {
        _alive.erase(id);

        for (auto& [type, anyStore] : _components) {
            auto& store = std::any_cast<std::unordered_map<EntityID, std::any>&>(anyStore);
            store.erase(id);
        }
    }

    inline bool Registry::alive(const EntityID id) const {
        return _alive.find(id) != _alive.end();
    }

    template<typename T>
    T& Registry::addComponent(const EntityID id, T component) {
        auto& store = _store<T>();
        store.emplace(id, std::move(component));
        return store.at(id);
    }

    template<typename T>
    T& Registry::getComponent(const EntityID id) {
        auto& store = _store<T>();
        auto  it    = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Entity does not have requested component.");
        return it->second;
    }

    template<typename T>
    const T& Registry::getComponent(const EntityID id) const {
        const auto& store = _store<T>();
        const auto  it    = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Entity does not have requested component.");
        return it->second;
    }

    template<typename T>
    T* Registry::tryGetComponent(const EntityID id) {
        auto& store = _store<T>();
        auto  it    = store.find(id);
        if (it == store.end()) return nullptr;
        return &it->second;
    }

    template<typename T>
    const T* Registry::tryGetComponent(const EntityID id) const {
        const auto& store = _store<T>();
        const auto  it    = store.find(id);
        if (it == store.end()) return nullptr;
        return &it->second;
    }

    template<typename T>
    bool Registry::hasComponent(const EntityID id) const {
        const auto& store = _store<T>();
        return store.find(id) != store.end();
    }

    template<typename T>
    void Registry::removeComponent(const EntityID id) {
        _store<T>().erase(id);
    }

    template<typename... Ts>
    void Registry::each(std::function<void(EntityID, Ts&...)> fn) {
        for (const EntityID id : _alive) {
            if ((hasComponent<Ts>(id) && ...))
                fn(id, getComponent<Ts>(id)...);
        }
    }

    template<typename... Ts, typename Fn>
    void Registry::each(Fn&& fn) {
        for (const EntityID id : _alive) {
            if ((hasComponent<Ts>(id) && ...))
                fn(id, getComponent<Ts>(id)...);
        }
    }

    inline const std::unordered_set<EntityID>& Registry::entities() const {
        return _alive;
    }

    /*
    template<typename T>
    std::unordered_map<EntityID, std::any>& Registry::_store() {
        return _components[std::type_index(typeid(T))];
    }

    template<typename T>
    const std::unordered_map<EntityID, std::any>& Registry::_store() const {
        return _components.at(std::type_index(typeid(T)));
    }
    */

    template<typename T>
    std::unordered_map<EntityID, T>& Registry::_store()
    {
        return std::any_cast<std::unordered_map<EntityID, T>&>(
            _components[std::type_index(typeid(T))]
        );
    }

    template<typename T>
    const std::unordered_map<EntityID, T>& Registry::_store() const
    {
        return std::any_cast<const std::unordered_map<EntityID, T>&>(
            _components.at(std::type_index(typeid(T)))
        );
    }


}
#endif
#endif

#endif //LATTICE_REGISTRY_H
