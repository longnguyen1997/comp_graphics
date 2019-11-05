#ifndef OBJPARSER_H
#define OBJPARSER_H

#include <string>
#include <vector>
#include <map>

#include <vecmath.h>

struct material {
    float shininess;
    Vector3f ambient;
    Vector3f diffuse; // ignored if there is diffuse texture
    Vector3f specular;
    std::string diffuse_texture;
};

// data is an array of RGB pixels.
// there is no alpha channel.
struct rgbimage {
    int w;
    int h;
    std::vector<uint8_t> data;
};

// a single obj file can contain multiple pieces of geometry.
// we refer to each piece as "batch", since it is drawn in 
// a single draw call.
// All triangles in a batch have the same material parameters.
struct draw_batch {
    std::string name; // useful for debugging
    int start_index;
    int nindices;
    material mat;
};

class objparser {
public:
    // return false on error
    bool parse(const std::string& objfile);
    void clear();

    // the parse() method fills these arrays with vertex, 
    // index, and texture data
    std::vector<Vector3f>           positions;
    std::vector<Vector3f>           normals;
    std::vector<Vector2f>           texcoords;

    std::vector<uint32_t>           indices;
    std::vector<draw_batch>         batches;
    std::map<std::string, rgbimage> textures;

private:
    // parse materials from .mtl file and store in materials map.
    bool parsemtl(const std::string& mtlfile, 
                  std::map<std::string, material> * materials);

    // parse textures referenced by mtl file.
    bool loadtextures(const std::string& basepath, 
                      const std::map<std::string, material>& materials);
};

#endif