#include "objparser.h"

#include <fstream>
#include <cstdio>
#include <sstream>
#include <cassert>

#include "stb_image.h"


void objparser::clear() {
    positions.clear();
    normals.clear();
    texcoords.clear();
    indices.clear();
    textures.clear();
    batches.clear();
}

bool objparser::parse(const std::string& objfile) {
    clear();

    std::fstream fh(objfile);
    if (!fh) {
        printf("Cannot open file %s\n", objfile.c_str());
        return false;
    }

    size_t last_sep = objfile.find_last_of("\\/");
    std::string basepath;
    if (last_sep == std::string::npos) {
        basepath = "";
    }
    else {
        basepath = objfile.substr(0, last_sep + 1);
    }

    std::map<std::string, material> materials;

    draw_batch current_batch;

    std::string line;
    while (std::getline(fh, line)) {
        std::stringstream ibuff(line);
        std::string command;
        ibuff >> command;
        if (command == "#" || command == "") {
            continue;
        }
        else if (command == "v") {
            Vector3f p;
            ibuff >> p.x() >> p.y() >> p.z();
            positions.push_back(p);
        }
        else if (command == "vt") {
            Vector2f uv;
            ibuff >> uv.x() >> uv.y();
            texcoords.push_back(uv);
        }
        else if (command == "vn") {
            Vector3f n;
            ibuff >> n.x() >> n.y() >> n.z();
            normals.push_back(n);
        }
        else if (command == "f") {
            uint32_t a, b, c;
            ibuff >> a >> b >> c;
            indices.push_back(a - 1);
            indices.push_back(b - 1);
            indices.push_back(c - 1);
        }
        else if (command == "g") {
            if (current_batch.name != "") {
                // end previous batch
                current_batch.nindices = (int)indices.size() - current_batch.start_index;
                batches.push_back(current_batch);
            }
            // start new batch
            current_batch.start_index = (int)indices.size();
            ibuff >> current_batch.name;
            printf("New geometry %s at face index %d\n", current_batch.name.c_str(), (int)indices.size());
        }
        else if (command == "usemtl") {
            std::string usemtl;
            ibuff >> usemtl;
            printf("Use material %s at face index %d\n", usemtl.c_str(), (int)indices.size());
            current_batch.mat = materials[usemtl];
        }
        else if (command == "mtllib") {
            std::string mtllib;
            ibuff >> mtllib;
            printf("Use material library %s%s\n", basepath.c_str(), mtllib.c_str());
            if (!parsemtl(basepath + mtllib, &materials)) {
                clear();
                return false;
            }
            if (!loadtextures(basepath, materials)) {
                clear();
                return false;
            }
        }
        else {
            printf("Unknown obj command: %s\n", command.c_str());
            return false;
        }
    }

    if (current_batch.name != "") {
        // end previous batch
        current_batch.nindices = (int)indices.size() - current_batch.start_index;
        batches.push_back(current_batch);
    }

    assert(positions.size() == normals.size());   // THIS IS NOT TRUE IN GENERAL FOR OBJ
    assert(positions.size() == texcoords.size()); // WE ASSUME A NORMALIZED FILE FORMAT
    return true;
}
bool objparser::parsemtl(const std::string& mtlfile, std::map<std::string, material> * materials) {
    std::fstream fh(mtlfile);
    if (!fh) {
        printf("Cannot open mtl file %s\n", mtlfile.c_str());
        return false;
    }

    std::string matname;
    material mat;

    std::string line;
    while (std::getline(fh, line)) {
        std::stringstream ibuff(line);
        std::string command;
        ibuff >> command;
        if (command == "#" || command == "") {
            continue;
        }
        else if (command == "newmtl") {
            if (matname != "") {
                materials->insert(std::make_pair(matname, mat));
                mat = material();
            }
            ibuff >> matname;
        }
        else if (command == "Ns") {
            ibuff >> mat.shininess;
        }
        else if (command == "Ka") {
            ibuff >> mat.ambient.x() >> mat.ambient.y() >> mat.ambient.z();
        }
        else if (command == "Kd") {
            ibuff >> mat.diffuse.x() >> mat.diffuse.y() >> mat.diffuse.z();
        }
        else if (command == "Ks") {
            ibuff >> mat.specular.x() >> mat.specular.y() >> mat.specular.z();
        }
        else if (command == "map_Kd") {
            ibuff >> mat.diffuse_texture;
        }
        else if (command == "map_bump") {
            // ignoring bump map
        }
        else {
            printf("Unknown MTL command %s\n", command.c_str());
        }
    }
    if (matname != "") {
        materials->insert(std::make_pair(matname, mat));
    }
    return true;
}
bool objparser::loadtextures(const std::string& basepath, const std::map<std::string, material>& materials) {
    for (auto it = materials.begin(); it != materials.end(); ++it) {
        material mat = it->second;
        if (mat.diffuse_texture != "") {
            std::string jpgfile = basepath + mat.diffuse_texture;
            //printf("Loading texture from %s\n", jpgfile.c_str());

            rgbimage im;
            int nc;
            uint8_t* imdata = stbi_load(jpgfile.c_str(), &im.w, &im.h, &nc, 3);
            if (!imdata || nc != 3) {
               printf("Loading texture from %s failed\n", jpgfile.c_str());
               return false;
            }
            im.data.resize(im.w * im.h * nc);
            std::copy(imdata, imdata + im.data.size(), im.data.begin());
            // note: this code id not the most efficient.
            // we first std::copy from the stbi pointer into the rgbimage on the stack.
            // Then, we copy the rgbimage into the map, which can copy the array again.
            textures.insert(std::make_pair(mat.diffuse_texture, im));
            //printf("Load Image w=%d h=%d nc = %d\n", w, h, nc); stbi_image_free(imdata);
        }
    }
    return true;
}
