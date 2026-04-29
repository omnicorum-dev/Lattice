//
// Created by Nico Russo on 4/29/26.
//

#include "registry.h"

namespace Lattice {
    EntityID Registry::createEntity() {
        EntityID id = _nextID++;
        _alive.insert(id);
        return id;
    }

    void Registry::destroyEntity(EntityID id) {
        for (auto& [type, storage] : _components)
            storage->destroy(id);
        _alive.erase(id);
    }

    bool Registry::alive(EntityID id) const {
        return _alive.count(id);
    }

    const std::unordered_set<EntityID>& Registry::entities() const {
        return _alive;
    }
}