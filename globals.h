//
// Created by Nico Russo on 4/28/26.
//

#ifndef LATTICE_GLOBALS_H
#define LATTICE_GLOBALS_H

#include <memory>
#define activeScene Lattice::ActiveScene::get()

class MediumOpenGL;

extern std::unique_ptr<MediumOpenGL> app;
extern const int GAME_WIDTH;
extern const int GAME_HEIGHT;

#endif //LATTICE_GLOBALS_H
