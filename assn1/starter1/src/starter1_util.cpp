#include "starter1_util.h"

#include "gl.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstring>

// defined later in this file
void setupDebugPrint();
void printOpenGLVersion();


GLFWwindow* createOpenGLWindow(int width, int height, const char* title) {
    // GLFW creates a window and OpenGL context
    // in a platform-independent manner.
    GLFWwindow* window;
    if (!glfwInit()) {
        printf("Could not init glfw\n");
        return nullptr;
    }

#ifdef __APPLE__
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GLFW_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#else
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif
    window = glfwCreateWindow(width, height, title, NULL, NULL);
    if (!window) {
        return nullptr;
    }
    // This must be called only once for each window
    glfwMakeContextCurrent(window);

    // You should be running an OpenGL 3.3 context
    printOpenGLVersion();

    // GLEW initializes the OpenGL functions
    // Without this, only a small subset of OpenGL's
    // functionality would be available
#ifndef __APPLE__
    GLenum err = glewInit();
    if (GLEW_OK != err) {
        //Problem: glewInit failed, something is wrong.
        fprintf(stderr, "Error: %s\n", glewGetErrorString(err));
        return nullptr;
    }
#endif

    // This line is optional, but very useful for debugging
#ifndef __APPLE__
    setupDebugPrint();
#endif
    return window;
}

#ifndef __APPLE__
static void GLAPIENTRY gl_dbg_callback(GLenum source,
    GLenum type,
    GLuint id,
    GLenum severity,
    GLsizei length,
    const GLchar* message,
    const void* userParam)
{
    if (severity > GL_DEBUG_SEVERITY_NOTIFICATION) {
        // below some spammy ids that you might want to filter 
        //id != 131204 && id != 131076 && id != 131184 && 
        //id != 131186 && id != 131188 && id != 131154
        if (id != 131076) {
            const char* msg = "Type = %d, id = %d, severity = %d, %s\n";
            fprintf(stderr, msg, type, id, severity, message);
        }
    }
}

void setupDebugPrint()
{
    glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE,
        GL_DONT_CARE, 0, nullptr, GL_TRUE);
    glDebugMessageCallback(gl_dbg_callback, nullptr);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
}
#endif

static bool compileShader(GLuint handle, GLenum stype, const char* src)
{
    int shader_len = (int)strlen(src);
    glShaderSource(handle, 1, &src, &shader_len);
    glCompileShader(handle);

    int status;
    glGetShaderiv(handle, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buff[2048];
        int nwritten;
        glGetShaderInfoLog(handle, 2048, &nwritten, buff);

        const char* typelabel = stype == GL_VERTEX_SHADER ? "vertex" : (stype == GL_FRAGMENT_SHADER ? "fragment" : "unknown");
        printf("Error in %s shader\n%s\n", typelabel, buff);
        return false;
    }
    return true;
}

// caller must free returned shader object
static int compileShader(GLenum type, const char* src)
{
    GLuint handle = glCreateShader(type);
    compileShader(handle, type, src);
    return handle;
}


static bool linkProgram(GLuint handle, GLuint vshader, GLuint fshader)
{
    glAttachShader(handle, vshader);
    glAttachShader(handle, fshader);
    glLinkProgram(handle);
    int success;
    glGetProgramiv(handle, GL_LINK_STATUS, &success);
    if (!success) {
        char buff[2048];
        int nwritten;
        glGetProgramInfoLog(handle, 2048, &nwritten, buff);
        fprintf(stderr, "Program link error:\n%s\n", buff);
        return false;
    }
    return true;
}

uint32_t compileProgram(const char* vshader_src, const char* fshader_src)
{
    GLuint program = glCreateProgram();
    GLuint vshader = compileShader(GL_VERTEX_SHADER, vshader_src);
    GLuint fshader = compileShader(GL_FRAGMENT_SHADER, fshader_src);
    if (!linkProgram(program, vshader, fshader)) {
        glDeleteProgram(program);
        program = 0;
    }
    // once a program is linked
    // shader objects should be deleted
    glDeleteShader(vshader);
    glDeleteShader(fshader);
    return program;
}

void printOpenGLVersion()
{
    int major;
    int minor;
    glGetIntegerv(GL_MAJOR_VERSION, &major);
    glGetIntegerv(GL_MINOR_VERSION, &minor);
    printf("Running OpenGL %d.%d\n", major, minor);
}

float deg2rad(float deg)
{
    return deg / 180.0f * 3.141592f;
}

float rad2deg(float rad)
{
    return rad / 3.141592f * 180.0f;
}

