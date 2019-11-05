#ifndef MAIN_H
#define MAIN_H

#include <GLFW/glfw3.h>
#include <iostream>

#include "starter5_util.h"
#include "gl.h"
#include "vecmath.h"
#include "vertexrecorder.h"
#include "camera.h"

// globals
GLFWwindow* window;

// shader programs
// see loadPrograms() and freePrograms()
GLuint program_quad;
GLuint program_color;
GLuint program_light;

// camera and coordinate axes
bool gMousePressed = false;
Camera    camera;

// Declarations of functions whose implementations occur later in main.h
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseCallback(GLFWwindow* window, int button, int action, int mods);
void motionCallback(GLFWwindow* window, double x, double y);

void drawAxis();
void drawTexturedQuad(GLint texture);
void setViewportWindow(GLFWwindow* window);

void updateMaterialUniforms(GLuint program, Vector3f diffuseColor,
    Vector3f ambientColor = Vector3f(-1, -1, -1),
    Vector3f specularColor = Vector3f(0, 0, 0),
    float shininess = 1.0f,
    float alpha = 1.0f);
void updateLightUniforms(GLuint program, Vector3f pos, Vector3f color = Vector3f(1, 1, 1));
void updateTransformUniforms(uint32_t program, Matrix4f M, Matrix4f V, Matrix4f P);

bool loadPrograms(const std::string & basepath);
void freePrograms();

class glfwtimer {
public:
    void set() {
        freq = glfwGetTimerFrequency();
        start = glfwGetTimerValue();
    }
    // return number of seconds elapsed
    float elapsed() {
        uint64_t now = glfwGetTimerValue();
        return (float)(now - start) / freq;
    }

    uint64_t freq;
    uint64_t start;

};

// ----------------------------------------------------------------------
// Details --- draw axes, key callbacks, etc.
// You don't have to edit anything below here.

// draw a texture onto the screen
void drawTexturedQuad(GLint tex) {
    glUseProgram(program_quad);
    updateTransformUniforms(program_quad, Matrix4f::identity(), Matrix4f::identity(), Matrix4f::identity());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, tex);

    glDisable(GL_DEPTH_TEST);
    drawUnitQuad();

    glEnable(GL_DEPTH_TEST);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void setViewportWindow(GLFWwindow* window)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    camera.SetDimensions(w, h);
    camera.SetViewport(0, 0, w, h);
    camera.ApplyViewport();
}



bool loadPrograms(const std::string & basepath) {
    // The program object controls the programmable parts
    // of OpenGL. All OpenGL programs define a vertex shader
    // and a fragment shader.
    std::string vshader = basepath + "shaders/vertexshader.glsl";
    std::string fshader_light = basepath + "shaders/fragmentshader_dirlight.glsl";
    std::string fshader_color = basepath + "shaders/fragmentshader_color.glsl";
    std::string fshader_quad = basepath + "shaders/diffuse_nolight.glsl";
    program_color = compileProgramFromFile(vshader.c_str(), fshader_color.c_str());
    if (!program_color) {
        printf("Cannot compile program\n");
        return false;
    }
    program_light = compileProgramFromFile(vshader.c_str(), fshader_light.c_str());
    if (!program_light) {
        printf("Cannot compile program\n");
        return false;
    }
    program_quad = compileProgramFromFile(vshader.c_str(), fshader_quad.c_str());
    if (!program_quad) {
        printf("Cannot compile program\n");
        return false;
    }
    return true;
}
void freePrograms() {
    glDeleteProgram(program_color); program_color = 0;
    glDeleteProgram(program_light); program_light = 0;
    glDeleteProgram(program_quad); program_quad = 0;
}

void updateMaterialUniforms(GLuint program, Vector3f diffuseColor,
    Vector3f ambientColor,
    Vector3f specularColor,
    float shininess,
    float alpha) {
    int loc = glGetUniformLocation(program, "diffColor");
    glUniform3fv(loc, 1, diffuseColor);
    if (ambientColor.x() <= 0) {
        ambientColor = 0.05f * diffuseColor;
    }
    loc = glGetUniformLocation(program, "ambientColor");
    glUniform3fv(loc, 1, ambientColor);
    loc = glGetUniformLocation(program, "specColor");
    glUniform3fv(loc, 1, specularColor);
    loc = glGetUniformLocation(program, "shininess");
    glUniform1f(loc, shininess);
    loc = glGetUniformLocation(program, "alpha");
    glUniform1f(loc, alpha);
}

void updateLightUniforms(GLuint program, Vector3f pos, Vector3f color) {
    int loc = glGetUniformLocation(program, "lightPos");
    glUniform3fv(loc, 1, pos);

    loc = glGetUniformLocation(program, "lightDiff");
    glUniform3fv(loc, 1, color);
}

void updateTransformUniforms(uint32_t program, Matrix4f M, Matrix4f V, Matrix4f P) {
    Matrix4f C = V.inverse();
    Vector3f eye = C.getCol(3).xyz();

    int loc = glGetUniformLocation(program, "P");
    glUniformMatrix4fv(loc, 1, false, P);

    loc = glGetUniformLocation(program, "V");
    glUniformMatrix4fv(loc, 1, false, V);

    loc = glGetUniformLocation(program, "camPos");
    glUniform3fv(loc, 1, eye);

    loc = glGetUniformLocation(program, "M");
    glUniformMatrix4fv(loc, 1, false, M);

    Matrix4f N = M.inverse().transposed();
    loc = glGetUniformLocation(program, "N");
    glUniformMatrix4fv(loc, 1, false, N);
}



void keyCallback(GLFWwindow* window, int key,
    int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) { // only handle PRESS and REPEAT
        return;
    }

    // Special keys (arrows, CTRL, ...) are documented
    // here: http://www.glfw.org/docs/latest/group__keys.html
    switch (key) {
        //case GLFW_KEY_ESCAPE: // Escape key
        //exit(0);
        //break;
    case ' ':
    {
        Matrix4f eye = Matrix4f::identity();
        camera.SetRotation(eye);
        camera.SetCenter(Vector3f(0, 0, 0));
        break;
    }
    default:
        std::cout << "Unhandled key press " << key << "." << std::endl;
    }
}

void mouseCallback(GLFWwindow* window, int button, int action, int mods)
{
    double xd, yd;
    glfwGetCursorPos(window, &xd, &yd);
    int x = (int)xd;
    int y = (int)yd;

    int lstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);
    int rstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT);
    int mstate = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_MIDDLE);
    if (lstate == GLFW_PRESS) {
        gMousePressed = true;
        camera.MouseClick(Camera::LEFT, x, y);
    }
    else if (rstate == GLFW_PRESS) {
        gMousePressed = true;
        camera.MouseClick(Camera::RIGHT, x, y);
    }
    else if (mstate == GLFW_PRESS) {
        gMousePressed = true;
        camera.MouseClick(Camera::MIDDLE, x, y);
    }
    else {
        gMousePressed = true;
        camera.MouseRelease(x, y);
        gMousePressed = false;
    }
}

void motionCallback(GLFWwindow* window, double x, double y)
{
    if (!gMousePressed) {
        return;
    }
    camera.MouseDrag((int)x, (int)y);
}

void drawAxis()
{
    glUseProgram(program_color);
    Matrix4f M = Matrix4f::translation(camera.GetCenter()).inverse();
    updateTransformUniforms(program_color, M, camera.GetViewMatrix(), camera.GetPerspective());

    const Vector3f DKRED(1.0f, 0.5f, 0.5f);
    const Vector3f DKGREEN(0.5f, 1.0f, 0.5f);
    const Vector3f DKBLUE(0.5f, 0.5f, 1.0f);
    const Vector3f GREY(0.5f, 0.5f, 0.5f);

    const Vector3f ORGN(0, 0, 0);
    const Vector3f AXISX(5, 0, 0);
    const Vector3f AXISY(0, 5, 0);
    const Vector3f AXISZ(0, 0, 5);

    VertexRecorder recorder;
    recorder.record_poscolor(ORGN, DKRED);
    recorder.record_poscolor(AXISX, DKRED);
    recorder.record_poscolor(ORGN, DKGREEN);
    recorder.record_poscolor(AXISY, DKGREEN);
    recorder.record_poscolor(ORGN, DKBLUE);
    recorder.record_poscolor(AXISZ, DKBLUE);

    recorder.record_poscolor(ORGN, GREY);
    recorder.record_poscolor(-AXISX, GREY);
    recorder.record_poscolor(ORGN, GREY);
    recorder.record_poscolor(-AXISY, GREY);
    recorder.record_poscolor(ORGN, GREY);
    recorder.record_poscolor(-AXISZ, GREY);

    glLineWidth(3);
    recorder.draw(GL_LINES);
}


#endif
