#ifndef STARTER1_UTIL_H
#define STARTER1_UTIL_H

#include <cstdint>
#include <string>
#include "gl.h"

float deg2rad(float deg);
float rad2deg(float rad);

struct GLFWwindow;
// creates a window using GLFW and initializes an OpenGL 3.3+ context.
GLFWwindow* createOpenGLWindow(int width, int height, const char* title);

// returns 0 on error
// program must be freed with glDeleteProgram()
uint32_t compileProgram(const char* vertexshader, const char* fragmentshader);
uint32_t compileProgramFromFile(const char* vertexshaderfile, const char* fragmentshaderfile);

std::string readfile(const std::string& fname);
#endif
