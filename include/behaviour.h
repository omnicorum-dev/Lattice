//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_BEHAVIOUR_H
#define LATTICE_BEHAVIOUR_H

#include "scene.h"

namespace Lattice {

    class Behaviour {
    public:
        EntityID entity = NULL_ENTITY;
        Scene* scene = nullptr;

    protected:
        template<typename T>
        T& getComponent() {
            if (!scene)
                throw std::runtime_error("Behaviour has no Scene assigned");

            if (entity == NULL_ENTITY)
                throw std::runtime_error("Behaviour has no Entity assigned");

            return scene->registry.getComponent<T>(entity);
        }

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

    template<typename T, typename... Args>
    T& addScript(Scene& s, const EntityID e, Args&&... args)
    {
        static_assert(std::is_base_of_v<Behaviour, T>,
            "addScript<T>: T must derive from Behaviour");

        if (!s.registry.hasComponent<Behaviours>(e)) {
            s.registry.addComponent<Behaviours>(e, Behaviours{});
        }

        auto& [list, started] = s.registry.getComponent<Behaviours>(e);

        auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

        ptr->scene = &s;
        ptr->entity = e;

        T& ref = *ptr;
        list.push_back(std::move(ptr));
        return ref;
    }

}

#endif //LATTICE_BEHAVIOUR_H
