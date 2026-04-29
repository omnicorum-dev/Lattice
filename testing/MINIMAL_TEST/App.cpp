//
// Created by Nico Russo on 4/28/26.
//
#include <hooks.h>
#include <lattice.h>
#include <globals.h>
#include <iostream>

const int GAME_WIDTH = 800;
const int GAME_HEIGHT = 600;

const std::string appName = "Minimal App";

LATTICE_HOOK(Start) {
    LOG_DEBUG("Start");
}

LATTICE_HOOK(Update, float dt) {
}

LATTICE_HOOK(LateUpdate, float dt) {
    activeScene.canvas.writeStringBaseline("Hello, World!", 20, 52, 32, Graphite::Colors::White);
}