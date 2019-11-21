#include "CubeMap.h"

#include <cmath>
#include <string>
#include <iostream>

CubeMap::CubeMap(const std::string &directory)
{
    std::string side[6] = { "left", "right", "up", "down", "front", "back" };
    for(int ii = 0 ;ii<6;ii++){
        std::string filename = directory + "/" + side[ii] + ".png";
        _images[ii] = Image::loadPNG(filename);
    }

}


Vector3f
CubeMap::getFaceTexel(float x, float y, int face) const
{
    x = x * _images[face].getWidth();
    y = (1 - y) * _images[face].getHeight();
    int ix = (int) x;
    int iy = (int) y;
    float alpha = x - ix;
    float beta = y - iy;

    const Vector3f &pixel0 = getTexturePixel(ix + 0, iy + 0, face);
    const Vector3f &pixel1 = getTexturePixel(ix + 1, iy + 0, face);
    const Vector3f &pixel2 = getTexturePixel(ix + 0, iy + 1, face);
    const Vector3f &pixel3 = getTexturePixel(ix + 1, iy + 1, face);

    Vector3f color;
    for (int ii = 0; ii < 3; ii++) {
        color[ii] = 
              (1 - alpha) * (1 - beta) * pixel0[ii]
            +      alpha  * (1 - beta) * pixel1[ii]
            + (1 - alpha) *      beta  * pixel2[ii]
            +      alpha  *      beta  * pixel3[ii];
    }

    return color;
}


Vector3f
CubeMap::getTexel(const Vector3f &direction) const
{
    Vector3f dir = direction.normalized();
    Vector3f outputColor(0.0f, 0.0f, 0.0f);
    if ((std::abs(dir[0]) >= std::abs(dir[1])) && (std::abs(dir[0]) >= std::abs(dir[2]))) {
        if (dir[0] > 0.0f) {
            outputColor = 
                getFaceTexel((dir[2] / dir[0] + 1.0f) * 0.5f, 
                                   (dir[1] / dir[0] + 1.0f) * 0.5f, RIGHT);
        } else if (dir[0] < 0.0f) {
            outputColor = 
                getFaceTexel(       (dir[2] / dir[0] + 1.0f) * 0.5f, 
                                  1.0f - (dir[1] / dir[0] + 1.0f) * 0.5f, LEFT);
        }
    } else if ((std::abs(dir[1]) >= std::abs(dir[0])) && (std::abs(dir[1]) >= std::abs(dir[2]))) {
        if (dir[1] > 0.0f) {
            outputColor = 
                getFaceTexel((dir[0] / dir[1] + 1.0f) * 0.5f, 
                                (dir[2] / dir[1] + 1.0f) * 0.5f, UP);
        } else if (dir[1] < 0.0f) {
            outputColor = 
                getFaceTexel(1.0f - (dir[0] / dir[1] + 1.0f) * 0.5f, 
                                  1.0f - (dir[2] / dir[1] + 1.0f) * 0.5f, DOWN);
        }
    } else if ((std::abs(dir[2]) >= std::abs(dir[0])) && (std::abs(dir[2]) >= std::abs(dir[1]))) {
        if (dir[2] > 0.0f) {
            outputColor = 
                getFaceTexel(1.0f - (dir[0] / dir[2] + 1.0f) * 0.5f, 
                                          (dir[1] / dir[2] + 1.0f) * 0.5f, FRONT);
        } else if (dir[2] < 0.0f) {
            outputColor = 
                getFaceTexel(       (dir[0] / dir[2] + 1.0f) * 0.5f, 
                                  1.0f - (dir[1] / dir[2] + 1.0f) * 0.5f, BACK);
        }
    }

    return outputColor;
}
