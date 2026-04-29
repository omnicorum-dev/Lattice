//
// Created by Nico Russo on 4/29/26.
//

#ifndef LATTICE_BEHAVIOUR_TEMPLATES_H
#define LATTICE_BEHAVIOUR_TEMPLATES_H

#include "behaviour.h"
#include "scene.h"

namespace Lattice {
    template<typename T>
    T& Behaviour::getComponent() {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        return entity.scene->registry.getComponent<T>(entity);
    }

    template<typename T>
    void Behaviour::addComponent(T&& component) {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        entity.scene->registry.addComponent(entity.id, std::forward<T>(component));
    }

    template<typename T>
    T* Behaviour::tryGetComponent() {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        return entity.scene->registry.tryGetComponent<T>(entity.id);
    }

    template<typename T>
    bool Behaviour::hasComponent() const {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        return entity.scene->registry.hasComponent<T>(entity.id);
    }

    template<typename T, typename... Args>
    T& Behaviour::addScript(Args&&... args) {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        return entity.scene->addScript<T>(entity.id, std::forward<Args>(args)...);
    }

    template<typename T>
    void Behaviour::removeComponent() const {
        if (!entity.scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity.id == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        entity.scene->registry.removeComponent<T>(entity.id);
    }
}

#endif //LATTICE_BEHAVIOUR_TEMPLATES_H
