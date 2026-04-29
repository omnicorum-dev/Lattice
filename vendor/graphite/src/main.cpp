//
// Created by Nico Russo on 4/15/26.
//
#include <graphite.h>
#include <base.h>
#include <random>

using namespace omni;

Graphite::Canvas mainCanvas(1920, 1080);

struct Object {
    Vec2<float> position = {};
    Vec2<float> velocity = {};
    float size = 2.f;
    Graphite::Color color = {};

    void draw(const Graphite::Canvas& canvas) const {
        canvas.fillCircle(position.x, position.y, size, color);
    }
};

constexpr i32 GRAVITY = 600;

std::vector<Object*> objects;

void startGame(int numBalls) {
    std::random_device rd;
    std::mt19937 gen(rd());

    f32 min_x_velo = -500.f;
    f32 max_x_velo = 500.f;
    f32 min_y_velo = -1000.f;
    f32 max_y_velo = 1000.f;

    std::uniform_real_distribution<float> x_pos_distro(0.f, (float)mainCanvas.getWidth());
    std::uniform_real_distribution<float> y_pos_distro(0.f, (float)mainCanvas.getHeight());
    std::uniform_real_distribution<float> x_vel_distro(min_x_velo, max_x_velo);
    std::uniform_real_distribution<float> y_vel_distro(min_y_velo, max_y_velo);
    std::uniform_int_distribution<int> rand_color(0, max_i32);

    for (int i = 0; i < numBalls; i++) {
        Vec2<float> position = {x_pos_distro(gen), y_pos_distro(gen)};
        Vec2<float> velocity = {x_vel_distro(gen), y_vel_distro(gen)};
        Graphite::Color color = rand_color(gen);
        color.a = 0xff;
        Object *new_ball = new Object {position, velocity, 3, color};
        objects.emplace_back(new_ball);
    }
}

void updateObject (Object* ball, const float dt) {
    constexpr f32 ENERGY_LOSS = 0.95;

    ball->position.x += ball->velocity.x * dt;
    ball->position.y += ball->velocity.y * dt;

    ball->size = length(ball->velocity) * 0.01f;

    if (ball->position.x <= 0 || ball->position.x >= mainCanvas.getWidth()) {
        ball->velocity.x = -ball->velocity.x * ENERGY_LOSS;
        ball->position.x = std::clamp(ball->position.x, 1.f, (float)mainCanvas.getWidth() - 1);
    }

    if (ball->position.y >= mainCanvas.getHeight()) {
        ball->velocity.y = -ball->velocity.y * ENERGY_LOSS;
        ball->position.y = std::clamp(ball->position.y, -1000.f, (float)mainCanvas.getHeight() - 1);
    }

    ball->velocity.y += GRAVITY * dt;
}

Graphite::Canvas& gameUpdate(f32 dt) {
    mainCanvas.fillFast(0xFF101010);

    for (Object* ball : objects) {
        updateObject(ball, dt);
        ball->draw(mainCanvas);
    }

    mainCanvas.writeString(stringPrint("FPS: {}", 1.f/dt), 10, 34, 24, 0xffcccccc);

    return mainCanvas;
}

int main() {

    startGame(1000);

    gameUpdate(0.8f);

    if (!mainCanvas.saveToPPM("../output.ppm")) {
        LOG_ERROR("Failed to save to ppm");
    }

    if (!mainCanvas.saveToJPG("../output.jpg")) {
        LOG_ERROR("Failed to save to jpg");
    }

    if (!mainCanvas.saveToPNG("../output.png")) {
        LOG_ERROR("Failed to save to png");
    }

    if (!mainCanvas.saveToBMP("../output.bmp")) {
        LOG_ERROR("Failed to save to bmp");
    }

    return 0;
}