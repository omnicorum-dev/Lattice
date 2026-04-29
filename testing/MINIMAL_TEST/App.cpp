//
// Created by Nico Russo on 4/28/26.
//
#include <hooks.h>
#include <lattice.h>
#include <globals.h>
#include "mediumOpenGL.h"

const int APP_WIDTH = 800;
const int APP_HEIGHT = 600;

using namespace Lattice;

class SphereBehaviour : public Behaviour {
private:
    Transform* transform = nullptr;
public:
    void Start() override {
        transform = scene->registry.tryGetComponent<Transform>(entity);
    }

    void Update(const float dt) override {
        if (transform) {
            transform->rotateAroundAxis(Dir3D::RIGHT, glm::radians(45.0f) * dt);
        }
    }
};

LATTICE_HOOK(LateUpdate, float dt) {
    activeScene.canvas.writeStringBaseline("Hello, World!", 20, 52, 32, Graphite::Colors::White);
}

LATTICE_HOOK(Start) {
    LOG_DEBUG("Start");
    app->setWindowName("APP");

    LOG_DEBUG("\nGAME_WIDTH: {}\nGAME_HEIGHT: {}\nSCREEN_WIDTH: {}\nSCREEN_HEIGHT: {}", app->GAME_WIDTH, app->GAME_HEIGHT, app->SCREEN_WIDTH, app->SCREEN_HEIGHT);

    std::filesystem::path assetPath = app->getAssetRoot();

    EntityID id = activeScene.createEntity("Camera");
    activeScene.registry.addComponent(id, Lattice::Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
        .nearPlane  = 0.1f,
        .farPlane   = 100.f
    });

    id = activeScene.createEntity("Sphere");
    activeScene.registry.addComponent(id, Transform{.position = {0, 0, 3}});
    activeScene.registry.addComponent(id, Primitives::sphere());
    addScript<SphereBehaviour>(activeScene, id);
}