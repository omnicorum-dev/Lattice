//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_SCENE_H
#define LATTICE_SCENE_H

#include "registry.h"
#include "scenegraph.h"
#include "signalbus.h"
#include "entity.h"

#include <input.h>

#include <globals.h>

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

        std::unordered_map<std::string, EntityID> nameIndex;
        std::unordered_map<std::string, std::vector<EntityID>> tagIndex;
        std::unordered_map<EntityID, Entity> entities;

        /*
        void clear() {
            //registry.clear();
            //sceneGraph.clear();
            // signals intentionally NOT cleared (depends on design)
        }
        */

        Scene() : canvas(GAME_WIDTH, GAME_HEIGHT), zBuffer(GAME_WIDTH, GAME_HEIGHT) {}

        [[nodiscard]] Input& getInput() const {
            return *input;
        }

        [[nodiscard]] Input* tryGetInput() const {
            return input;
        }

        EntityID createEntity(const std::string& name = "")
        {
            const EntityID id = registry.createEntity();

            entities[id] = Entity{ id, name };

            if (!name.empty()) {
                nameIndex[name] = id;
            }

            return id;
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

#endif //LATTICE_SCENE_H
