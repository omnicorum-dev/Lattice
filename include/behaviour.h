//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_BEHAVIOUR_H
#define LATTICE_BEHAVIOUR_H

#include <vector>
#include <memory>
#include <stdexcept>
#include "entity.h"

namespace Lattice {
    struct Scene;

    class Behaviour {
    public:
        //EntityID entity = NULL_ENTITY;
        //Scene*   scene  = nullptr;
        Entity entity;

    protected:
        template<typename T>
        T& getComponent();  // defined in behaviour_templates.h, included from scene.h

        template<typename T>
        void addComponent(T&& component);

        template<typename T>
        T* tryGetComponent();

        template<typename T>
        bool hasComponent() const;

        template<typename T>
        void removeComponent() const;

        template<typename T, typename... Args>
        T& addScript(Args&&... args);

    public:
        virtual void Start()          {}
        virtual void Update(float dt) {}
        virtual void Destroy()        {}
        virtual ~Behaviour() = default;
    };

    struct Behaviours {
        std::vector<std::unique_ptr<Behaviour>> list;
        bool started = false;
    };
}

#endif //LATTICE_BEHAVIOUR_H
