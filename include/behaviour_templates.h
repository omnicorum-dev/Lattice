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
        if (!scene)                throw std::runtime_error("Behaviour has no Scene assigned");
        if (entity == NULL_ENTITY) throw std::runtime_error("Behaviour has no Entity assigned");
        return scene->registry.getComponent<T>(entity);
    }
}

#endif //LATTICE_BEHAVIOUR_TEMPLATES_H
