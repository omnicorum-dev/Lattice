//
// Created by Nico Russo on 4/28/26.
//

#define NO_STB_IMPL
#include <../../vendor/medium/include/mediumOpenGL.h>
#include <../../vendor/medium/include/inputGLFW.h>
#include <lattice.h>

#include <globals.h>
#include <hooks.h>

extern void UpdateEarly(float dt);
extern void Update(float dt);
extern void UpdateAfterDraw(float dt);

std::unique_ptr<MediumOpenGL> app = nullptr;

void frameLoop(const float dt) {
    if (Lattice::Hooks::Update)
        Lattice::Hooks::Update(dt);

    // fixedUpdate???

    activeScene.registry.each<Lattice::Behaviours> (
    [&](const Lattice::EntityID id, Lattice::Behaviours& behaviour)
    {
        bool runStart = false;
        if (!behaviour.started) {
            runStart = true;
            behaviour.started = true;
        }

        for (const auto& s : behaviour.list)
        {
            if (runStart)
            {
                s->entity.id = id;
                s->Start();
            }
            s->Update(dt);
        }
    });

    Lattice::Camera3D* activeCamera = nullptr;
    Lattice::EntityID cameraID = activeScene.findByName("Camera");
    if (cameraID == Lattice::NULL_ENTITY) {
        LOG_ERROR("No active camera found");
    } else {
        activeCamera = &activeScene.registry.getComponent<Lattice::Camera3D>(cameraID);
    }

    activeScene.canvas.fill(activeScene.clearColor);
    activeScene.zBuffer.clear();

    if (activeCamera) {
        activeScene.registry.each<Lattice::Transform, Lattice::Model3D>(
            [&](Lattice::EntityID id, const Lattice::Transform& t, const Lattice::Model3D& model) {
                Lattice::Render3D::renderFlat(activeScene.canvas, t, model, *activeCamera, Graphite::Colors::LightBlue, true, Lattice::Dir3D::DOWN, &(activeScene.zBuffer));
            }
        );
    }

    if (Lattice::Hooks::LateUpdate)
        Lattice::Hooks::LateUpdate(dt);

    app->renderCanvas(activeScene.canvas);
}

void startup() {
    if (Lattice::Hooks::Start) {
        Lattice::Hooks::Start();
    }

    if (activeScene.findByName("Camera") == Lattice::NULL_ENTITY) {
        const Lattice::EntityID cameraID = activeScene.createEntity("Camera");
        activeScene.registry.addComponent(cameraID, Lattice::Camera3D{});
    }
}

void shutdown() {
    if (Lattice::Hooks::Shutdown)
        Lattice::Hooks::Shutdown();
}

// Entry Point
int main() {
    app = std::make_unique<MediumOpenGL>(APP_WIDTH, APP_HEIGHT, APP_WIDTH, APP_HEIGHT);
    app->mediumStartup();
    Lattice::Scene scene;
    InputGLFW input;
    scene.input = &input;
    scene.input->initializeInput(app.get());
    Lattice::ActiveScene::set(&scene);
    startup();
    app->mediumRun(frameLoop);
    shutdown();
    app->mediumShutdown();
    return 0;
}