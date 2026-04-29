//
// Created by Nico Russo on 4/28/26.
//
#include <hooks.h>
#include <lattice.h>
#include <globals.h>
#include <mediumOpenGL.h>

const int APP_WIDTH = 1920/4;
const int APP_HEIGHT = 1080/4;

using namespace Lattice;

class ObjectBehaviour : public Behaviour {
private:
    Transform* transform = nullptr;
public:
    void Start() override {
        transform = scene->registry.tryGetComponent<Transform>(entity);
    }

    void Update(const float dt) override {
        if (transform) {
            transform->rotateAroundAxis(Dir3D::UP, glm::radians(45.0f) * dt);
        }
    }
};


LATTICE_HOOK(LateUpdate, float dt) {
    static float smoothFps = 0.f;
    float fps = 1.f / dt;
    smoothFps += (fps - smoothFps) * 0.01f;
    char buf[256];
    sprintf(buf, "fps: %3.0f", smoothFps);
    std::string s = buf;
    activeScene.canvas.writeStringBaseline(s, 20, 52-16, 16, Graphite::Colors::White);
}

LATTICE_HOOK(Start) {
    app->setWindowName("APP");

    const std::filesystem::path assetPath = app->getAssetRoot();

    GLuint crtShader = MediumOpenGL::buildShader(assetPath / "crt.frag");
    app->setScreenShader(crtShader);

    activeScene.clearColor = Graphite::Colors::DarkGrey;

    EntityID id = activeScene.createEntity("Camera");
    activeScene.registry.addComponent(id, Lattice::Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
        .nearPlane  = 0.1f,
        .farPlane   = 100.f
    });

    id = activeScene.createEntity("Car");
    activeScene.registry.addComponent(id, Transform{.position = {0, -3, 15}});
    activeScene.registry.addComponent(id, loadOBJ(assetPath / "Car.obj"));
    addScript<ObjectBehaviour>(activeScene, id);
}