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



#if 0
#include <string>

#include "glm/fwd.hpp"

namespace Lattice {
    using EntityID = glm::u32;

    struct Entity {
        EntityID id;
        std::string name;
        Scene* scene;

        template<typename T>
        void addComponent(T&& component) {
            scene->registry.addComponent(id, std::forward<T>(component));
        }

        template<typename T>
        T& getComponent() {
            return scene->registry.getComponent<T>(id);
        }

        template<typename T>
        bool hasComponent() const {
            return scene->registry.hasComponent<T>(id);
        }

        template<typename T, typename... Args>
        T& addScript(Args&&... args)
        {
            static_assert(std::is_base_of_v<Behaviour, T>,
                "addScript<T>: T must derive from Behaviour");

            if (!scene->registry.hasComponent<Behaviours>(e)) {
                scene->registry.addComponent<Behaviours>(e, Behaviours{});
            }

            auto& [list, started] = scene->registry.getComponent<Behaviours>(e);

            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

            ptr->scene = scene;
            ptr->entity = e;

            T& ref = *ptr;
            list.push_back(std::move(ptr));
            return ref;
        }

        void addTag(const std::string& tag) {
            scene->tagIndex[tag].push_back(id);
        }

        void destroyEntity()
        {
            // remove from name index
            auto entIt = scene->entities.find(id);
            if (entIt != scene->entities.end()) {
                scene->nameIndex.erase(entIt->second.name);
            }

            // remove from tag index
            for (auto&[fst, snd] : tagIndex)
            {
                std::vector<EntityID>& vec = snd;

                for (auto it = vec.begin(); it != vec.end(); )
                {
                    if (*it == id)
                        it = vec.erase(it);
                    else
                        ++it;
                }
            }

            scene->entities.erase(id);
            scene->registry.destroyEntity(id);
        }
    };
}
#endif

#endif //LATTICE_ENTITY_H
