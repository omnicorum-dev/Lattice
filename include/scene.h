//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_SCENE_H
#define LATTICE_SCENE_H

#include "registry.h"
#include "scenegraph.h"
#include "signalbus.h"
#include "entity.h"
#include "behaviour.h"
#include <input.h>
#include <globals.h>
#include <unordered_map>
#include <vector>
#include <string>
#include <stdexcept>
#include <algorithm>

namespace Lattice {
    struct Scene {
        Registry   registry;
        SceneGraph sceneGraph;
        SignalBus  signalBus;

        Input* input = nullptr;

        Graphite::Canvas canvas;
        Graphite::Canvas zBuffer;
        Graphite::Color  clearColor = {0xff000000};

        std::unordered_map<std::string, EntityID>              nameIndex;
        std::unordered_map<std::string, std::vector<EntityID>> tagIndex;
        std::unordered_map<EntityID, Entity>                   entities;

        Scene() : canvas(APP_WIDTH, APP_HEIGHT), zBuffer(APP_WIDTH, APP_HEIGHT) {}

        [[nodiscard]] Input& getInput() const { return *input; }
        [[nodiscard]] Input* tryGetInput() const { return input; }

        template<typename T>
        void addComponent(EntityID id, T&& component) {
            registry.addComponent(id, std::forward<T>(component));
        }

        template<typename T>
        T& getComponent(EntityID id) {
            return registry.getComponent<T>(id);
        }

        template<typename T>
        bool hasComponent(EntityID id) const {
            return registry.hasComponent<T>(id);
        }

        template<typename T, typename... Args>
        T& addScript(EntityID e, Args&&... args)
        {
            static_assert(std::is_base_of_v<Behaviour, T>,
                "addScript<T>: T must derive from Behaviour");

            if (!registry.hasComponent<Behaviours>(e))
                registry.addComponent<Behaviours>(e, Behaviours{});

            auto& [list, started] = registry.getComponent<Behaviours>(e);

            auto ptr    = std::make_unique<T>(std::forward<Args>(args)...);
            ptr->scene  = this;
            ptr->entity = e;

            T& ref = *ptr;
            list.push_back(std::move(ptr));
            return ref;
        }

        Entity& createEntity(const std::string& name = "");
        [[nodiscard]] EntityID findByName(const std::string& name) const;
        Entity* getEntity(EntityID id);
        void addTag(EntityID id, const std::string& tag);
        [[nodiscard]] std::vector<EntityID> findByTag(const std::string& tag) const;
        void destroyEntity(EntityID id);
    };

    class ActiveScene {
    public:
        static void set(Scene* scene) { current() = scene; }

        static Scene& get() {
            Scene* ptr = current();
            if (!ptr) throw std::runtime_error("ActiveScene::get() called with no active scene set");
            return *ptr;
        }

        static Scene* tryGet() { return current(); }

    private:
        static Scene*& current() {
            thread_local Scene* instance = nullptr;
            return instance;
        }
    };
}

// templates that need Scene fully defined
#include "entity_templates.h"
#include "behaviour_templates.h"

#endif //LATTICE_SCENE_H
