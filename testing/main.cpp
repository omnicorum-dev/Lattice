#include <iostream>

#include <mediumOpenGL.h>
#include <inputGLFW.h>

void globalEventCallback(int key, int action, int mods, double x, double y) {}

#define WIDTH 800
#define HEIGHT 600

MediumOpenGL game(WIDTH, HEIGHT, WIDTH, HEIGHT, "Lattice Example");
Input* Input::instance = nullptr;
std::map<std::pair<int,int>, Input::EventCallback> Input::eventCallbacks;
Input::GlobalEventCallback Input::globalCallback;

Graphite::Canvas canvas(WIDTH,HEIGHT);

void update(float dt) {
    canvas.fill(0xff181818);
    canvas.writeStringBaseline("Hello, World!", WIDTH/2 - 200, HEIGHT/2, 32, Graphite::Colors::White);
    game.renderCanvas(canvas);
}

void start() {
    Input::instance = new InputGLFW(&game);
    Input::registerGlobalCallback(globalEventCallback);
}

int main() {
    LOG_INFO("Hello {}", 69);
    game.mediumStartup();
    start();
    game.mediumRun(update);
    game.mediumShutdown();
    delete Input::instance;
    return 0;
}
