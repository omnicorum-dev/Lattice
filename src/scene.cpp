//
// Created by Nico Russo on 4/29/26.
//
#include "scene.h"

namespace Lattice {
    Entity& Scene::createEntity(const std::string& name)
    {
        const EntityID id = registry.createEntity();
        entities[id] = Entity{ id, name, this };
        if (!name.empty()) nameIndex[name] = id;
        return entities[id];
    }

    EntityID Scene::findByName(const std::string& name) const
    {
        const auto it = nameIndex.find(name);
        return it == nameIndex.end() ? NULL_ENTITY : it->second;
    }

    Entity* Scene::getEntity(const EntityID id)
    {
        const auto it = entities.find(id);
        return it == entities.end() ? nullptr : &it->second;
    }

    void Scene::addTag(const EntityID id, const std::string& tag)
    {
        tagIndex[tag].push_back(id);
    }

    std::vector<EntityID> Scene::findByTag(const std::string& tag) const
    {
        const auto it = tagIndex.find(tag);
        return it == tagIndex.end() ? std::vector<EntityID>{} : it->second;
    }

    void Scene::destroyEntity(EntityID id)
    {
        auto entIt = entities.find(id);
        if (entIt != entities.end())
            nameIndex.erase(entIt->second.name);

        for (auto& [tag, vec] : tagIndex)
            vec.erase(std::remove(vec.begin(), vec.end(), id), vec.end());

        entities.erase(id);
        registry.destroyEntity(id);
    }
}