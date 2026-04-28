#include <mediumOpenGL.h>
#include <inputGLFW.h>
#include <primitives.h>
#include <registry.h>
#include <render3D.h>

using Lattice::EntityID;

#define WIDTH (800/2)
#define HEIGHT (600/2)

MediumOpenGL game(WIDTH*2, HEIGHT*2, WIDTH, HEIGHT, "Lattice Example");
InputGLFW input;

std::filesystem::path assetPath = game.getAssetRoot();
std::filesystem::path savePath = game.getSaveRoot();

Lattice::Registry   registry;
Lattice::SceneGraph graph;

EntityID cubeID = registry.createEntity();
EntityID groundID = registry.createEntity();
EntityID cameraID = registry.createEntity();

Shader crt = 0;

Graphite::Canvas canvas(WIDTH,HEIGHT);
Graphite::Canvas zBuffer(WIDTH,HEIGHT);

constexpr float walkSpeed   = 5.f;
constexpr float sprintSpeed = 15.f;
constexpr float turnSpeed = glm::radians(60.f);

void update(float dt) {

    // PREPARE

    auto* camera = registry.tryGetComponent<Lattice::Camera3D>(cameraID);
    auto* cubeTrans = registry.tryGetComponent<Lattice::Transform>(cubeID);

    if (camera == nullptr) {
        LOG_ERROR("No Lattice::Camera3D found with ID {}", cameraID);
        camera = new Lattice::Camera3D();
    }

    Lattice::Transform& cameraTrans = camera->transform;

    // INPUT

    glm::vec3 movementVector{};
    float movementSpeed = walkSpeed;

    if (input.isKeyPressed(MED_KEY_LEFT_SHIFT)) { movementSpeed = sprintSpeed; }

    if (input.isKeyPressed(MED_KEY_W)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::FORWARD); }
    if (input.isKeyPressed(MED_KEY_S)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::BACKWARD); }
    if (input.isKeyPressed(MED_KEY_D)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::RIGHT); }
    if (input.isKeyPressed(MED_KEY_A)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::LEFT); }
    if (input.isKeyPressed(MED_KEY_E)) { movementVector += Lattice::Dir3D::UP; }
    if (input.isKeyPressed(MED_KEY_Q)) { movementVector += Lattice::Dir3D::DOWN; }

    /*
    if (input.isKeyPressed(MED_KEY_L)) { cameraTrans.rotateAroundAxis(Lattice::Dir3D::UP,  turnSpeed * dt); }
    if (input.isKeyPressed(MED_KEY_J)) { cameraTrans.rotateAroundAxis(Lattice::Dir3D::UP, -turnSpeed * dt); }
    constexpr float pitchLimit = glm::radians(85.f);

    if (input.isKeyPressed(MED_KEY_I)) {
        const glm::vec3 forward      = cameraTrans.localDirection(Lattice::Dir3D::FORWARD);
        const float     currentPitch = glm::asin(glm::clamp(-forward.y, -1.f, 1.f));
        if (currentPitch > -pitchLimit)
            cameraTrans.rotateAroundAxis(cameraTrans.localDirection(Lattice::Dir3D::RIGHT), -turnSpeed * dt);
    }
    if (input.isKeyPressed(MED_KEY_K)) {
        const glm::vec3 forward      = cameraTrans.localDirection(Lattice::Dir3D::FORWARD);
        const float     currentPitch = glm::asin(glm::clamp(-forward.y, -1.f, 1.f));
        if (currentPitch < pitchLimit)
            cameraTrans.rotateAroundAxis(cameraTrans.localDirection(Lattice::Dir3D::RIGHT),  turnSpeed * dt);
    }
    */

    cameraTrans.lookAt(cubeTrans->position);

    if (glm::length(movementVector) >= 0.001f) {
        movementVector = normalize(movementVector);
    }

    movementVector *= movementSpeed;

    // OBJECT UPDATES

    cameraTrans.position += movementVector * dt;

    cubeTrans->rotateAroundAxis(Lattice::Dir3D::UP, dt * glm::radians(90.f));
    cubeTrans->rotateAroundAxis(Lattice::Dir3D::RIGHT, dt * glm::radians(45.f));

    // RENDERING

    canvas.fill(0xff181818);
    zBuffer.clear();

    canvas.writeStringBaseline("Hello, World!", WIDTH/2 - 100, HEIGHT/2, 16, Graphite::Colors::White);
    const float frameRate = 1.f/dt;
    std::string s = "fps: " + std::to_string(frameRate);

    registry.each<Lattice::Transform, Lattice::Model3D>(
        [&](EntityID id, const Lattice::Transform& t, const Lattice::Model3D& model) {
            //Lattice::Render3D::renderWireframe(canvas, t, model, *camera, 1, Graphite::Colors::Blue);
            Lattice::Render3D::renderFlat(canvas, t, model, *camera, Graphite::Colors::LightBlue, true, Lattice::Dir3D::UP, &zBuffer);
            //Lattice::Render3D::renderPoints(canvas, t, model, *camera, 1, Graphite::Colors::Red);
        }
    );

    canvas.writeStringBaseline(s, 10, 20, 16, Graphite::Colors::White);

    game.renderCanvas(canvas);
}

void start() {
    input.initializeInput(&game);
    LOG_DEBUG("Asset Path: {}", assetPath);
    crt = MediumOpenGL::buildShader(assetPath / "crt.frag");
    game.setScreenShader(crt);

    registry.addComponent(cubeID, Lattice::Transform{
        .position = {0.f, 0.f, 3.f}
    });
    registry.addComponent(cubeID, Lattice::Primitives::cylinder());

    registry.addComponent(groundID, Lattice::Transform{
        .position = {0.f, -3.f, 0.f},
        .scale = {2.f, 2.f, 2.f}}
    );
    registry.addComponent(groundID, Lattice::Primitives::grid());

    const EntityID s1 = registry.createEntity();
    registry.addComponent(s1, Lattice::Transform{
        .position = {3.f, 0.f, 3.f}
    });
    registry.addComponent(s1, Lattice::Primitives::cube());

    const EntityID s2 = registry.createEntity();
    registry.addComponent(s2, Lattice::Transform{
        .position = {3.f, 0.f, 5.f}
    });
    registry.addComponent(s2, Lattice::Primitives::cube());

    const EntityID s3 = registry.createEntity();
    registry.addComponent(s3, Lattice::Transform{
        .position = {0.f, -1.f, 3.f}
    });
    registry.addComponent(s3, Lattice::Primitives::cube());


    registry.addComponent(cameraID, Lattice::Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
        .nearPlane  = 0.1f,
        .farPlane   = 100.f
    });
}

int main() {
    LOG_INFO("Hello {}", 69);
    game.mediumStartup();
    start();
    game.mediumRun(update);
    game.mediumShutdown();
    return 0;
}
