#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cassert>

#include "Image.h"

#include "stb_image.h"
#include "stb_image_write.h"

static
uint8_t
clampColorComponent(float c)
{
    int tmp = int(c * 255);

    if (tmp < 0) {
        tmp = 0;
    }

    if (tmp > 255) {
        tmp = 255;
    }

    return uint8_t(tmp);
}

void
Image::savePNG(const std::string &filename) const
{
    assert(!filename.empty());

    std::vector<uint8_t> buffer;
    buffer.resize(_width * _height * 3);

    // flip y so that (0,0) is bottom left corner
    for (int c = 0, y = _height - 1; y >= 0; y--) {
        for (int x = 0; x < _width; x++) {
            const Vector3f &pixel = getPixel(x, y);
            buffer[c++] = clampColorComponent(pixel[0]);
            buffer[c++] = clampColorComponent(pixel[1]);
            buffer[c++] = clampColorComponent(pixel[2]);
        }
    }

    stbi_write_png(filename.c_str(), _width, _height, 3, &buffer[0], _width * 3);
}

Image 
Image::loadPNG(const std::string &filename) 
{
    assert(!filename.empty());

    int w, h, n;
    unsigned char *buffer = stbi_load(filename.c_str(), &w, &h, &n, 0);
    assert(buffer != NULL);
    assert(n == 3);

    Image image(w, h);

    // flip y so that (0,0) is bottom left corner
    for (int c = 0, p = 0, y = h - 1; y >= 0; y--) {
        for (int x = 0; x < w; x++, ++p) {
            Vector3f &pixel = image._data[p];
            pixel[0] = buffer[c++] / 255.0f;
            pixel[1] = buffer[c++] / 255.0f;
            pixel[2] = buffer[c++] / 255.0f;
        }
    }
    stbi_image_free(buffer);

    return image;
}

Image
Image::compare(const Image& img1, const Image & img2) 
{
    assert(img1.getWidth() == img2.getWidth());
    assert(img1.getHeight() == img2.getHeight());

    Image diff(img1.getWidth(), img1.getHeight());

    const int width = img1.getWidth();
    const int height = img1.getHeight();
    for (int x = 0; x < width; x++) {
        for (int y = 0; y < height; y++) {
            const Vector3f &color1 = img1.getPixel(x, y);
            const Vector3f &color2 = img2.getPixel(x, y);
            Vector3f color3 =
                Vector3f(fabs(color1[0] - color2[0]),
                         fabs(color1[1] - color2[1]),
                         fabs(color1[2] - color2[2]));
            diff.setPixel(x, y, color3);
        }
    }

    return diff;
}
