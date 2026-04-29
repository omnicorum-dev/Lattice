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
        EntityID entity = NULL_ENTITY;
        Scene*   scene  = nullptr;

    protected:
        template<typename T>
        T& getComponent();  // defined in behaviour_templates.h, included from scene.h

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

#if 0
#include <vector>

namespace Lattice {

    struct Scene;

    class Behaviour {
    public:
        EntityID entity = NULL_ENTITY;
        Scene* scene = nullptr;

    protected:
        template<typename T>
        T& getComponent(); // defined in BehaviourImpl.h

    public:
        virtual void Start() {}
        virtual void Update(float dt) {}
        virtual void Destroy() {}

        virtual ~Behaviour() = default;
    };

    struct Behaviours {
        std::vector<std::unique_ptr<Behaviour>> list;
        bool started = false;
    };

}
#endif

#endif //LATTICE_BEHAVIOUR_H
