//
// Created by Nico Russo on 4/29/26.
//
#include <app_include.h>

using namespace Lattice;

constexpr int APP_WIDTH = 400;
constexpr int APP_HEIGHT = 300;

class SphereScript : public Behaviour {
    Transform* transform = nullptr;
public:
    void Start() override {
        transform = tryGetComponent<Transform>();
    }

    void Update(float dt) override {
        if (transform) {
            transform->rotateAroundAxis(Dir3D::FORWARD, glm::radians(45.f) * dt);
            transform->rotateAroundAxis(Dir3D::RIGHT, glm::radians(90.f) * dt);
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
    activeScene.canvas.writeStringBaseline(s, 20, 52-16, 16, Graphite::Colors::Red);
}

LATTICE_HOOK(Start) {
    app->setWindowName("Fun App!!!!");

    std::filesystem::path assetsPath = app->getAssetRoot();

    GLuint shader = MediumOpenGL::buildShader(assetsPath / "testFrag.glsl");
    //app->setScreenShader(shader);

    Entity entity = activeScene.createEntity("Sphere");
    entity.addComponent(Transform{.position = {0., -2., 8.}});
    entity.addComponent(loadOBJ(assetsPath / "Car.obj"));
    entity.addScript<SphereScript>();
}