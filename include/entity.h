//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_ENTITY_H
#define LATTICE_ENTITY_H

#include <string>
#include "glm/fwd.hpp"

namespace Lattice {
    using EntityID = glm::u32;
    //constexpr EntityID NULL_ENTITY = 0;

    struct Scene;

    struct Entity {
        EntityID    id    = NULL_ENTITY;
        std::string name;
        Scene*      scene = nullptr;

        operator EntityID() const { return id; }

        template<typename T>
        void addComponent(T&& component);

        template<typename T>
        T& getComponent();

        template<typename T>
        bool hasComponent() const;

        template<typename T, typename... Args>
        T& addScript(Args&&... args);

        void addTag(const std::string& tag);
        void destroy();
    };

    // templates defined in entity_templates.h, included from scene.h
}

#endif //LATTICE_ENTITY_H
