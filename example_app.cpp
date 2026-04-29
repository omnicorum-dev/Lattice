//
// Created by Nico Russo on 4/29/26.
//
#include <app_include.h>

const int APP_WIDTH = 800;
const int APP_HEIGHT = 600;

using namespace Lattice;

LATTICE_HOOK(Start) {
    LOG_DEBUG("Hello from Example App!");

    app->setWindowName("Example App");

    std::filesystem::path assetPath = app->getAssetRoot();

    Entity entity = activeScene.createEntity("Car");
    entity.addComponent(Transform{
        .position = {0., -3., 10.},
        .rotation = glm::angleAxis(glm::radians(-120.0f), glm::vec3(0, 1, 0))
    });
    entity.addComponent(loadOBJ(assetPath / "Car.obj"));

    entity = activeScene.createEntity("Camera");
    entity.addComponent(Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
    });
}