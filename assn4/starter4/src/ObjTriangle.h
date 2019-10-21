#ifndef OBJ_TRIANGLE_H
#define OBJ_TRIANGLE_H

#include <array>

// By default counterclockwise winding is front face
struct ObjTriangle {
    ObjTriangle() :
        x{ { 0, 0, 0 } },
        texID{ { 0, 0, 0 } }
    {
    }

    int & operator[](int i) {
        return x[i];
    }

    std::array<int, 3> x;
    std::array<int, 3> texID;
};

#endif // OBJ_TRIANGLE_H
