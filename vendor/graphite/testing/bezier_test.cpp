//
// Created by Nico Russo on 4/16/26.
//
#include <graphite.h>
#include <base.h>
#include <random>

using namespace omni;

Graphite::Canvas mainCanvas(800, 600);

int main() {
    mainCanvas.fill(Graphite::Colors::DarkGrey);

    mainCanvas.writeStringBaseline("Bezier + Line Testing", 10, 34, 24, Graphite::Colors::LightGrey);

    const Vec2<float> p1 = {100, 400};
    const Vec2<float> p2 = {400, 100};
    const Vec2<float> p3 = {700, 500};

    mainCanvas.drawPoint(p1.x, p1.y, 4, Graphite::Colors::Red);
    mainCanvas.drawPoint(p2.x, p2.y, 4, Graphite::Colors::Red);
    mainCanvas.drawPoint(p3.x, p3.y, 4, Graphite::Colors::Red);

    Vec2<float> prev = {p1.x, p1.y};

    constexpr int numPoints = 25;

    for (float t = 0; t <= numPoints; ++t) {
        Vec2<float> p = bezierQuadratic(p1, p2, p3, (float)t/numPoints);
        //mainCanvas.drawPoint(p.x, p.y, 5, Graphite::Colors::Green);
        mainCanvas.drawLine(prev.x, prev.y, p.x, p.y, Graphite::Colors::Cyan, 3);
        prev = p;
    }

    if (!mainCanvas.saveToJPG("../testing/bezier.jpg")) {
        LOG_ERROR("Failed to save bezier image");
        return 1;
    }

    return 0;
}