//
// Created by Nico Russo on 4/29/26.
//
#include <app_include.h>

const int APP_WIDTH = 800;
const int APP_HEIGHT = 600;

using namespace Lattice;

class ObjectBehaviour : public Behaviour {
private:
    Transform* transform = nullptr;
public:
    void Start() override {
        transform = tryGetComponent<Transform>();
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
    LOG_DEBUG("Hello from Example App!");

    app->setWindowName("Example App");

    std::filesystem::path assetPath = app->getAssetRoot();

    Entity entity = activeScene.createEntity("Car");
    entity.addComponent(Transform{
        .position = {0., -3., 10.},
        .rotation = glm::angleAxis(glm::radians(-120.0f), glm::vec3(0, 1, 0))
    });
    entity.addComponent(loadOBJ(assetPath / "Car.obj"));
    entity.addScript<ObjectBehaviour>();

    entity = activeScene.createEntity("Camera");
    entity.addComponent(Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
    });
}