//
// Created by Nico Russo on 4/28/26.
//
#include <hooks.h>
#include <lattice.h>
#include <globals.h>
#include <iostream>

LATTICE_HOOK(Start) {
    std::cout << "Start" << std::endl;
}