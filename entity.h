//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_ENTITY_H
#define LATTICE_ENTITY_H

#include <string>

#include "glm/fwd.hpp"

namespace Lattice {
    using EntityID = glm::u32;

    struct Entity {
        EntityID id;
        std::string name;
    };
}

#endif //LATTICE_ENTITY_H
