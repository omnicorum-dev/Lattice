//
// Created by Nico Russo on 4/27/26.
//

#ifndef LATTICE_REGISTRY_H
#define LATTICE_REGISTRY_H

#include <any>
#include <typeindex>
#include <unordered_set>
#include <glm/glm.hpp>

#include "primitives.h"

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
        std::unordered_map<std::type_index,
            std::unordered_map<EntityID, std::any>>                  _components;

        template<typename T>
        std::unordered_map<EntityID, std::any>& _store();

        template<typename T>
        const std::unordered_map<EntityID, std::any>& _store() const;
    };


    // ==================== SCENE GRAPH ====================

    /**
     * @brief A node in the scene hierarchy, wrapping an EntityID.
     *
     * SceneNode manages parent/child relationships between entities.
     * World-space transforms are computed by composing transforms up
     * the chain from root to node. The Registry is the source of truth
     * for Transform components — SceneNode only stores the relationships.
     *
     * ### Typical usage
     * @code
     * SceneGraph graph;
     * EntityID parent = registry.createEntity();
     * EntityID child  = registry.createEntity();
     * registry.addComponent(parent, Transform{});
     * registry.addComponent(child,  Transform{});
     *
     * graph.setParent(child, parent);
     * Transform world = graph.worldTransform(child, registry);
     * @endcode
     */
    struct SceneGraph {
        /**
         * @brief Set the parent of an entity.
         *
         * Removes any existing parent relationship first. Passing NULL_ENTITY
         * detaches the entity from its parent, making it a root node.
         *
         * @param child  Entity to reparent.
         * @param parent New parent entity, or NULL_ENTITY to detach.
         */
        void setParent(EntityID child, EntityID parent);

        /**
         * @brief Detach an entity from its parent, making it a root node.
         * @param id Entity to detach.
         */
        void detach(EntityID id);

        /**
         * @brief Get the parent of an entity.
         * @param id Entity to query.
         * @return Parent EntityID, or NULL_ENTITY if this is a root node.
         */
        [[nodiscard]] EntityID parent(EntityID id) const;

        /**
         * @brief Get the direct children of an entity.
         * @param id Entity to query.
         * @return Set of child EntityIDs. Empty if the entity has no children.
         */
        [[nodiscard]] const std::unordered_set<EntityID>& children(EntityID id) const;

        /**
         * @brief Check whether an entity has a parent.
         * @param id Entity to query.
         * @return `true` if the entity has a parent.
         */
        [[nodiscard]] bool hasParent(EntityID id) const;

        /**
         * @brief Compute the world-space transform of an entity.
         *
         * Walks up the parent chain composing transforms via operator*.
         * Entities without a Transform component are treated as identity.
         * Result is cached — call invalidate() when a transform changes.
         *
         * @param id       Entity to query.
         * @param registry Registry to read Transform components from.
         * @return World-space transform.
         */
        [[nodiscard]] Transform worldTransform(EntityID id, const Registry& registry);

        /**
         * @brief Mark an entity's cached world transform as dirty.
         *
         * Call this whenever a Transform component is modified directly.
         * Also invalidates all descendants.
         *
         * @param id Entity whose transform changed.
         */
        void invalidate(EntityID id);

        /**
         * @brief Remove an entity from the scene graph entirely.
         *
         * Children are detached and become root nodes rather than being
         * destroyed — destroy them explicitly via the Registry if needed.
         *
         * @param id Entity to remove.
         */
        void remove(EntityID id);

    private:
        std::unordered_map<EntityID, EntityID>                     _parents;
        std::unordered_map<EntityID, std::unordered_set<EntityID>> _children;
        std::unordered_map<EntityID, Transform>                    _worldCache;
        std::unordered_set<EntityID>                               _dirty;
        inline static const std::unordered_set<EntityID>           _emptyChildren{};

        void _invalidateRecursive(EntityID id);
    };
}










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
        for (auto& [type, store] : _components)
            store.erase(id);
    }

    inline bool Registry::alive(const EntityID id) const {
        return _alive.find(id) != _alive.end();
    }

    template<typename T>
    T& Registry::addComponent(const EntityID id, T component) {
        auto& store = _store<T>();
        store[id]   = std::move(component);
        return std::any_cast<T&>(store.at(id));
    }

    template<typename T>
    T& Registry::getComponent(const EntityID id) {
        auto& store = _store<T>();
        auto  it    = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Entity does not have requested component.");
        return std::any_cast<T&>(it->second);
    }

    template<typename T>
    const T& Registry::getComponent(const EntityID id) const {
        const auto& store = _store<T>();
        const auto  it    = store.find(id);
        if (it == store.end())
            throw std::runtime_error("Entity does not have requested component.");
        return std::any_cast<const T&>(it->second);
    }

    template<typename T>
    T* Registry::tryGetComponent(const EntityID id) {
        auto& store = _store<T>();
        auto  it    = store.find(id);
        if (it == store.end()) return nullptr;
        return &std::any_cast<T&>(it->second);
    }

    template<typename T>
    const T* Registry::tryGetComponent(const EntityID id) const {
        const auto& store = _store<T>();
        const auto  it    = store.find(id);
        if (it == store.end()) return nullptr;
        return &std::any_cast<const T&>(it->second);
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

    template<typename T>
    std::unordered_map<EntityID, std::any>& Registry::_store() {
        return _components[std::type_index(typeid(T))];
    }

    template<typename T>
    const std::unordered_map<EntityID, std::any>& Registry::_store() const {
        return _components.at(std::type_index(typeid(T)));
    }


    // ==================== SCENE GRAPH ====================

    inline void SceneGraph::setParent(const EntityID child, const EntityID parent) {
        detach(child);
        if (parent == NULL_ENTITY) return;
        _parents[child] = parent;
        _children[parent].insert(child);
        _invalidateRecursive(child);
    }

    inline void SceneGraph::detach(const EntityID id) {
        const auto it = _parents.find(id);
        if (it == _parents.end()) return;
        _children[it->second].erase(id);
        _parents.erase(it);
        _invalidateRecursive(id);
    }

    inline EntityID SceneGraph::parent(const EntityID id) const {
        const auto it = _parents.find(id);
        return it != _parents.end() ? it->second : NULL_ENTITY;
    }

    inline const std::unordered_set<EntityID>& SceneGraph::children(const EntityID id) const {
        const auto it = _children.find(id);
        return it != _children.end() ? it->second : _emptyChildren;
    }

    inline bool SceneGraph::hasParent(const EntityID id) const {
        return _parents.find(id) != _parents.end();
    }

    inline Transform SceneGraph::worldTransform(const EntityID id, const Registry& registry) {
        // Return cached result if clean.
        if (_dirty.find(id) == _dirty.end()) {
            const auto it = _worldCache.find(id);
            if (it != _worldCache.end()) return it->second;
        }

        const Transform local = registry.hasComponent<Transform>(id)
            ? registry.getComponent<Transform>(id)
            : Transform{};

        Transform world = local;
        if (hasParent(id))
            world = worldTransform(_parents.at(id), registry) * local;

        _worldCache[id] = world;
        _dirty.erase(id);
        return world;
    }

    inline void SceneGraph::invalidate(const EntityID id) {
        _invalidateRecursive(id);
    }

    inline void SceneGraph::remove(const EntityID id) {
        // Detach children — they become root nodes.
        if (_children.find(id) != _children.end()) {
            for (const EntityID child : _children.at(id))
                _parents.erase(child);
            _children.erase(id);
        }
        detach(id);
        _worldCache.erase(id);
        _dirty.erase(id);
    }

    inline void SceneGraph::_invalidateRecursive(const EntityID id) {
        _dirty.insert(id);
        _worldCache.erase(id);
        if (_children.find(id) == _children.end()) return;
        for (const EntityID child : _children.at(id))
            _invalidateRecursive(child);
    }
}
#endif

#endif //LATTICE_REGISTRY_H
