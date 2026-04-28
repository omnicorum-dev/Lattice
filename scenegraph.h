//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_SCENEGRAPH_H
#define LATTICE_SCENEGRAPH_H

#include "primitives.h"
#include "registry.h"

namespace Lattice {
    using EntityID = glm::u32;
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
        if (_dirty.count(id) == 0 && _worldCache.count(id)) {
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

#endif //LATTICE_SCENEGRAPH_H
