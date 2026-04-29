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

#if 0

#include "registry.h"
#include "scenegraph.h"
#include "signalbus.h"
#include "entity.h"

#include <input.h>
#include <globals.h>

#include <behaviour.h>

#include <unordered_map>
#include <vector>
#include <string>

namespace Lattice {
    struct Scene {
        Registry   registry;
        SceneGraph sceneGraph;
        SignalBus  signalBus;

        Input* input = nullptr;

        Graphite::Canvas canvas;
        Graphite::Canvas zBuffer;

        Graphite::Color clearColor = {0xff000000};

        std::unordered_map<std::string, EntityID> nameIndex;
        std::unordered_map<std::string, std::vector<EntityID>> tagIndex;
        std::unordered_map<EntityID, Entity> entities;

        Scene() : canvas(APP_WIDTH, APP_HEIGHT), zBuffer(APP_WIDTH, APP_HEIGHT) {}

        [[nodiscard]] Input& getInput() const {
            return *input;
        }

        [[nodiscard]] Input* tryGetInput() const {
            return input;
        }

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

            if (!registry.hasComponent<Behaviours>(e)) {
                registry.addComponent<Behaviours>(e, Behaviours{});
            }

            auto& [list, started] = registry.getComponent<Behaviours>(e);

            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);

            ptr->scene = this;
            ptr->entity = e;

            T& ref = *ptr;
            list.push_back(std::move(ptr));
            return ref;
        }

        EntityID createEntity(const std::string& name = "")
        {
            const EntityID id = registry.createEntity();

            entities[id] = Entity{ id, name, this };

            if (!name.empty()) {
                nameIndex[name] = id;
            }

            return id;
            //return entities[id];
        }

        [[nodiscard]] EntityID findByName(const std::string& name) const
        {
            const auto it = nameIndex.find(name);
            if (it == nameIndex.end())
                return NULL_ENTITY;
            return it->second;
        }

        Entity* getEntity(const EntityID id)
        {
            const auto it = entities.find(id);
            if (it == entities.end()) return nullptr;
            return &it->second;
        }

        void addTag(const EntityID id, const std::string& tag) {
            tagIndex[tag].push_back(id);
        }

        [[nodiscard]] std::vector<EntityID> findByTag(const std::string& tag) const {
            const auto it = tagIndex.find(tag);
            if (it == tagIndex.end()) return {};
            return it->second;
        }

        void destroyEntity(EntityID id)
        {
            // remove from name index
            auto entIt = entities.find(id);
            if (entIt != entities.end()) {
                nameIndex.erase(entIt->second.name);
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

            entities.erase(id);
            registry.destroyEntity(id);
        }
    };

    class ActiveScene {
    public:
        // Set the active scene
        static void set(Scene* scene) {
            current() = scene;
        }

        // Get the active scene (safe)
        static Scene& get() {
            Scene* ptr = current();
            if (!ptr)
                throw std::runtime_error("ActiveScene::get() called with no active scene set");
            return *ptr;
        }

        // Try get (nullable)
        static Scene* tryGet() {
            return current();
        }

    private:
        // thread-local storage for future multithreading
        static Scene*& current() {
            thread_local Scene* instance = nullptr;
            return instance;
        }
    };

}

#include "behaviourImpl.h"

#endif

#endif //LATTICE_SCENE_H
