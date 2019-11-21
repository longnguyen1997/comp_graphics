#ifndef CUBEMAP_H
#define CUBEMAP_H

#include "Image.h"
#include "Vector3f.h"

#include <string>
#include "Vector3f.h"
#include <iostream>

class CubeMap {
public:
    enum FACE {
        LEFT,
        RIGHT,
        UP,
        DOWN,
        FRONT,
        BACK,
    };

    // Assumes a directory containing {left,right,up,down,front,back}.png
    CubeMap(const std::string &directory);

    // Returns color for given directory
    Vector3f getTexel(const Vector3f &direction) const;

    // The UV (x, y) coordinates are assumed to be normalized between 0 and 1.
    // The resulting look up is box filtered in the local 2x2 neighborhood.
    Vector3f getFaceTexel(float x, float y, int face) const;

private:
    Image _images[6];

    template<typename T>
    static T
        clamp(const T &v, const T &lower_range, const T &upper_range)
    {
        if (v < lower_range) {
            return lower_range;
        }
        else if (v > upper_range) {
            return upper_range;
        }
        else {
            return v;
        }
    }

    const Vector3f & getTexturePixel(int x, int y, int face) const {
        x = clamp(x, 0, _images[face].getWidth() - 1);
        y = clamp(y, 0, _images[face].getHeight() - 1);
        return _images[face].getPixel(x, y);
    }

};

#endif
