#ifndef RENDERER_H
#define RENDERER_H

#include <string>

class renderer {
public:
    // the renderer will load data and shaders relative to the basepath
    renderer(const std::string& basepath);

private:
    std::string basepath;
};

#endif
