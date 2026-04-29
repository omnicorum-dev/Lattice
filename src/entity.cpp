//
// Created by Nico Russo on 4/29/26.
//
#include "scene.h" // full definition needed here

namespace Lattice {
    void Entity::addTag(const std::string& tag) {
        scene->addTag(id, tag);
    }

    void Entity::destroy() {
        scene->destroyEntity(id);
    }
}