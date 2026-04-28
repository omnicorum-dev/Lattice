//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_LATTICE_H
#define LATTICE_LATTICE_H

#include <behaviour.h>
#include <primitives.h>
#include <registry.h>
#include <render3D.h>
#include <scene.h>
#include <scenegraph.h>
#include <signalbus.h>

namespace Lattice {
    struct EntityDestroyed {
        EntityID id;
    };

    inline void destroyEntity(Scene& scene, EntityID id)
    {
        // ---- 1. call script Destroy() ----
        if (auto* scriptList = scene.registry.tryGetComponent<Behaviours>(id))
        {
            std::vector<Behaviour*> scripts;
            scripts.reserve(scriptList->list.size());

            for (auto& uptr : scriptList->list)
                scripts.push_back(uptr.get());

            for (auto* s : scripts)
                if (s)
                    s->Destroy();
        }

        // ---- 2. emit event (optional but good) ----
        scene.signalBus.emit(EntityDestroyed{ id });

        // ---- 3. actually destroy data ----
        scene.registry.destroyEntity(id);
    }
}


#endif //LATTICE_LATTICE_H
