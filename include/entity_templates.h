//
// Created by Nico Russo on 4/29/26.
//

#ifndef LATTICE_ENTITY_TEMPLATES_H
#define LATTICE_ENTITY_TEMPLATES_H

#include "entity.h"

namespace Lattice {
    template<typename T>
    void Entity::addComponent(T&& component) {
        scene->addComponent(id, std::forward<T>(component));
    }

    template<typename T>
    T* Entity::tryGetComponent() {
        return scene->tryGetComponent<T>(id);
    }

    template<typename T>
    T& Entity::getComponent() {
        return scene->getComponent<T>(id);
    }

    template<typename T>
    bool Entity::hasComponent() const {
        return scene->hasComponent<T>(id);
    }

    template<typename T, typename... Args>
    T& Entity::addScript(Args&&... args) {
        return scene->addScript<T>(id, std::forward<Args>(args)...);
    }

    template<typename T>
    void Entity::removeComponent() const {
        scene->removeComponent<T>(id);
    }
}

#endif //LATTICE_ENTITY_TEMPLATES_H
