#include <mediumOpenGL.h>
#include <inputGLFW.h>
#include <lattice.h>


class CylinderBehaviour : public Lattice::Behaviour {
private:
    Lattice::Transform* transform = nullptr;
public:
    void Start() override {
        LOG_INFO("Cylinder Start() function");
        if (scene == nullptr) {
            LOG_ERROR("CylinderBehaviour scene is nullptr!");
            return;
        } else {
            LOG_INFO("CylinderBehaviour scene is valid");
        }

        transform = &getComponent<Lattice::Transform>();
    }

    void Update(float dt) override {
        transform->rotateAroundAxis(Lattice::Dir3D::UP, dt * glm::radians(90.f));
        transform->rotateAroundAxis(Lattice::Dir3D::RIGHT, dt * glm::radians(45.f));
    }
};

class CameraBehaviour : public Lattice::Behaviour {
private:
    Lattice::Camera3D* camera = nullptr;
    Input* input = nullptr;

    float movementSpeed = 0.f;

    Lattice::EntityID lookAtTarget = Lattice::NULL_ENTITY;

    const float walkSpeed = 5.f;
    const float sprintSpeed = 15.f;
public:
    void Start() override {
        LOG_INFO("Camera Start() function");
        if (scene == nullptr) {
            LOG_ERROR("Camera scene is nullptr!");
            return;
        } else {
            LOG_INFO("Camera scene is valid");
        }

        camera = &getComponent<Lattice::Camera3D>();

        input = scene->input;
        if (input == nullptr) {
            LOG_ERROR("Camera input is nullptr!");
        } else {
            LOG_INFO("Camera input is valid");
        }
    }

    void Update(float dt) override {
        Lattice::Transform& cameraTrans = camera->transform;

        if (lookAtTarget == Lattice::NULL_ENTITY) {
            lookAtTarget = scene->findByName("Cube");
        }

        glm::vec3 movementVector{};
        movementSpeed = walkSpeed;

        if (input->isKeyPressed(MED_KEY_LEFT_SHIFT)) { movementSpeed = sprintSpeed; }

        if (input->isKeyPressed(MED_KEY_W)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::FORWARD); }
        if (input->isKeyPressed(MED_KEY_S)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::BACKWARD); }
        if (input->isKeyPressed(MED_KEY_D)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::RIGHT); }
        if (input->isKeyPressed(MED_KEY_A)) { movementVector += cameraTrans.yawDirection(Lattice::Dir3D::LEFT); }
        if (input->isKeyPressed(MED_KEY_E)) { movementVector += Lattice::Dir3D::UP; }
        if (input->isKeyPressed(MED_KEY_Q)) { movementVector += Lattice::Dir3D::DOWN; }

        glm::vec3 target{0.,0., 8.};
        if (lookAtTarget != Lattice::NULL_ENTITY &&
    scene->registry.hasComponent<Lattice::Transform>(lookAtTarget))
        {
            target = scene->registry.getComponent<Lattice::Transform>(lookAtTarget).position;
        }

        cameraTrans.lookAt(target);

        if (glm::length(movementVector) >= 0.001f) {
            movementVector = normalize(movementVector);
        }

        movementVector *= movementSpeed;

        cameraTrans.position += movementVector * dt;
    }
};

using Lattice::EntityID;

#define WIDTH (800/2)
#define HEIGHT (600/2)

MediumOpenGL game(WIDTH*2, HEIGHT*2, WIDTH, HEIGHT, "Lattice Example");
InputGLFW input;

std::filesystem::path assetPath = game.getAssetRoot();
std::filesystem::path savePath = game.getSaveRoot();

Lattice::Scene* scene;

EntityID cameraID = Lattice::NULL_ENTITY;

Shader crt = 0;

Graphite::Canvas canvas(WIDTH,HEIGHT);
Graphite::Canvas zBuffer(WIDTH,HEIGHT);

constexpr float walkSpeed   = 5.f;
constexpr float sprintSpeed = 15.f;
constexpr float turnSpeed = glm::radians(60.f);

void update(float dt) {

    // PREPARE
    auto* camera = scene->registry.tryGetComponent<Lattice::Camera3D>(cameraID);
    if (camera == nullptr) {
        LOG_ERROR("No Lattice::Camera3D found with ID {}", cameraID);
        camera = new Lattice::Camera3D();
    }
    Lattice::Transform& cameraTrans = camera->transform;

    // Run Objects

    scene->registry.each<Lattice::Behaviours>(
    [&](const EntityID id, Lattice::Behaviours& behaviour)
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
                s->entity = id;
                s->Start();
            }
            s->Update(dt);
        }
    });

    // RENDERING

    canvas.fill(0xff181818);
    zBuffer.clear();

    canvas.writeStringBaseline("Hello, World!", WIDTH/2 - 100, HEIGHT/2, 16, Graphite::Colors::White);
    const float frameRate = 1.f/dt;
    std::string s = "fps: " + std::to_string(frameRate);

    scene->registry.each<Lattice::Transform, Lattice::Model3D>(
        [&](EntityID id, const Lattice::Transform& t, const Lattice::Model3D& model) {
            //Lattice::Render3D::renderWireframe(canvas, t, model, *camera, 1, Graphite::Colors::Blue);
            Lattice::Render3D::renderFlat(canvas, t, model, *camera, Graphite::Colors::LightBlue, true, Lattice::Dir3D::UP, &zBuffer);
            Lattice::Render3D::renderPoints(canvas, t, model, *camera, 1, Graphite::Colors::Red);
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

    scene = &Lattice::ActiveScene::get();
    scene->input = &input;

    EntityID tmp = scene->createEntity("Cube");
    scene->registry.addComponent(tmp, Lattice::Transform{
        .position = {0.f, 0.f, 3.f}
    });
    scene->registry.addComponent(tmp, Lattice::Primitives::cylinder());
    Lattice::addScript<CylinderBehaviour>(*scene, tmp);

    tmp = scene->createEntity("Ground");
    scene->registry.addComponent(tmp, Lattice::Transform{
        .position = {0.f, -3.f, 0.f},
        .scale = {2.f, 2.f, 2.f}}
    );
    scene->registry.addComponent(tmp, Lattice::Primitives::grid());

    tmp = scene->createEntity("s1");
    scene->registry.addComponent(tmp, Lattice::Transform{
        .position = {3.f, 0.f, 3.f}
    });
    scene->registry.addComponent(tmp, Lattice::Primitives::cube());

    tmp = scene->createEntity("s2");
    scene->registry.addComponent(tmp, Lattice::Transform{
        .position = {3.f, 0.f, 5.f}
    });
    scene->registry.addComponent(tmp, Lattice::Primitives::cube());

    tmp = scene->createEntity("s3");
    scene->registry.addComponent(tmp, Lattice::Transform{
        .position = {0.f, -1.f, 3.f}
    });
    scene->registry.addComponent(tmp, Lattice::Primitives::cube());

    tmp = scene->createEntity("Camera");
    scene->registry.addComponent(tmp, Lattice::Camera3D{
        .transform  = { .position = {0.f, 0.f, 0.f} },
        .fov        = glm::radians(60.f),
        .nearPlane  = 0.1f,
        .farPlane   = 100.f
    });
    Lattice::addScript<CameraBehaviour>(*scene, tmp);

    cameraID = scene->findByName("Camera");
    if (cameraID == Lattice::NULL_ENTITY)
        throw std::runtime_error("Camera not found");
}

int main() {
    LOG_INFO("Hello {}", 69);
    Lattice::Scene s;
    Lattice::ActiveScene::set(&s);
    game.mediumStartup();
    start();
    game.mediumRun(update);
    game.mediumShutdown();
    return 0;
}
