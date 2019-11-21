#include <cstring>
#include <iostream>

#include "ArgParser.h"
#include "Renderer.h"

int
main(int argc, const char *argv[])
{
    // Report help usage if no args specified.
    if (argc == 1) {
        std::cout << "Usage: a5 <args>\n"
            << "\n"
            << "Args:\n"
            << "\t-input <scene>\n"
            << "\t-size <width> <height>\n"
            << "\t-output <image.png>\n"
            << "\t[-depth <depth_min> <depth_max> <depth_image.png>\n]"
            << "\t[-normals <normals_image.png>]\n"
            << "\t[-bounces <max_bounces>\n]"
            << "\t[-shadows\n]"
            << "\n"
            ;
        return 1;
    }

    ArgParser argsParser(argc, argv);
    Renderer renderer(argsParser);
    renderer.Render();
    return 0;
}
