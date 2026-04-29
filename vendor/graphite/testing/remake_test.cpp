//
// Created by Nico Russo on 4/19/26.
//

#define PROFILING 1
#include <graphite.h>



int main() {
    omni::basic::Benchmark::Get().BeginSession("render", "../testing/results.json");
    const Graphite::Canvas canvas(800, 600);
    const Graphite::Canvas texture("../testing/output.ppm");
    const Graphite::Canvas zBuffer(800, 600);

    canvas.fill(Graphite::Colors::LightGrey);

    canvas.writeOmniStringBaseline("A B C D E F G H I J K L M \n"
                                   "N O P Q R S T U V W X Y Z \n"
                                   "a b c d e f g h i j k l m \n"
                                   "n o p q r s t u v w x y z \n"
                                   "0 1 2 3 4 5 6 7 8 9 \n"
                                   ". , ! ? : ; * ( ) [ ] { } \n"
                                   "\' \" ` ~ _ $", 10, 80, 6);



    canvas.fillTriangle(
        400, 100,   // top
        150, 500,   // bottom-left
        650, 500,   // bottom-right
        Graphite::Colors::Red,
        1.0f, 1.0f, 1.0f,                            // invZ per vertex
        &zBuffer    // no z-buffer
    );


    canvas.fillTriangleUV(
        400, 100,   // top
        150, 500,   // bottom-left
        650, 500,   // bottom-right
        {0.5f, 0.0f}, {0.0f, 1.0f}, {1.0f, 1.0f},  // uvOverZ (== uv when invZ=1)
        1.0f, 1.0f, 1.0f,                            // invZ per vertex
        texture,
        &zBuffer,    // no z-buffer
        -1.0f       // no lerp
    );

    if (!canvas.saveToJPG("../testing/new_output.jpg")) {
        omni::LOG_ERROR("Failed to save to JPEG");
        return 1;
    }

    omni::basic::Benchmark::Get().EndSession();

    return 0;
}