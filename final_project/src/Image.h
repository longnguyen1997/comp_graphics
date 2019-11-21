#ifndef IMAGE_H
#define IMAGE_H

#include <cassert>
#include <string>
#include <vector>

#include "vecmath.h"

// Simple image class
class Image
{
public:
    Image() : _width(0), _height(0) {}
    // Instantiate an image of given width and height
    // All pixels are set to black (0, 0, 0) by default.
    Image(int w, int h)
    {
        _width = w;
        _height = h;
        _data.resize(_width * _height);
    }

    // Return width of image
    int getWidth() const {
        return _width;
    }

    // Return height of image
    int getHeight() const {
        return _height;
    }

    // Set pixel to given RGB
    void setPixel(int x, int y, const Vector3f &color) {
        assert(x >= 0 && x < _width);
        assert(y >= 0 && y < _height);
        _data[y * _width + x] = color;
    }

    // Return pixel at given x, y coordinates
    const Vector3f & getPixel(int x, int y) const {
        assert(x >= 0 && x < _width);
        assert(y >= 0 && y < _height);
        return _data[y * _width + x];
    }

    // Initialize all pixels in image to given RGB color.
    void setAllPixels(const Vector3f &color) {
        for (int i = 0; i < _width * _height; ++i) {
            _data[i] = color;
        }
    }

    // Reads PNG image and return new image instance.
    static Image loadPNG(const std::string &filename);

    // Save contents of image to given file name in PNG file format.
    void savePNG(const std::string &filename) const;

    // Return an absolute difference betweenthe given images
    static Image compare(const Image & img1, const Image & img2);

private:
    int _width;
    int _height;
    std::vector<Vector3f> _data;
};

#endif // IMAGE_H
