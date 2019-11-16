#include "gl.h"
#include <GLFW/glfw3.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <vector>
#include <lodepng.h>
#include <map>
#include <cstdint>

#include "objparser.h"

// some utility code is tucked away in main.h
// for example, drawing the coordinate axes
// or helpers for setting uniforms.
#include "main.h"

// 4096x4096 is a pretty large texture. Extensions to shadow algorithm
// (extra credit) help lowering this memory footprint.
const int SHADOW_WIDTH = 4096;
const int SHADOW_HEIGHT = 4096;

// FUNCTION DECLARATIONS - you will implement these
void loadTextures();
void freeTextures();

void loadFramebuffer();
void freeFramebuffer();

void draw();

Matrix4f getLightView();
Matrix4f getLightProjection();

// Globals here.
objparser scene;
Vector3f  light_dir;
glfwtimer timer;


// FUNCTION IMPLEMENTATIONS

// animate light source direction
// this one is implemented for you
void updateLightDirection() {
    // feel free to edit this
    float elapsed_s = timer.elapsed();
    //elapsed_s = 88.88f;
    float timescale = 0.1f;
    light_dir = Vector3f(2.0f * sinf((float)elapsed_s * 1.5f * timescale),
                         5.0f, 2.0f * cosf(2 + 1.9f * (float)elapsed_s * timescale));
    light_dir.normalize();
}


void drawScene(GLint program, Matrix4f V, Matrix4f P) {
    VertexRecorder recorder;
    Matrix4f I = Matrix4f::identity();
    updateTransformUniforms(program, I, V, P);
    for (draw_batch batch : scene.batches) {
        int i = batch.start_index;
        while (i < batch.start_index + batch.nindices) {
            int j = scene.indices[i];
            recorder.record(scene.positions[j], scene.normals[j], Vector3f(scene.texcoords[j], 0.0));
            ++i;
        }
        updateMaterialUniforms(program,
                               batch.mat.diffuse,
                               batch.mat.ambient,
                               batch.mat.specular,
                               batch.mat.shininess);
        recorder.draw();
    }
}

void draw() {
    // 2. DEPTH PASS
    // - bind framebuffer
    // - configure viewport
    // - compute camera matrices (light source as camera)
    // - call drawScene

    // 1. LIGHT PASS
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    int winw, winh;
    glfwGetFramebufferSize(window, &winw, &winh);
    glViewport(0, 0, winw, winh);
    glUseProgram(program_light);
    updateLightUniforms(program_light, light_dir, Vector3f(1.2f, 1.2f, 1.2f));
    // TODO IMPLEMENT drawScene
    drawScene(program_light, camera.GetViewMatrix(), camera.GetPerspective());

    // 3. DRAW DEPTH TEXTURE AS QUAD
    // drawTexturedQuad() helper in main.h is useful here.
}

// Main routine.
// Set up OpenGL, define the callbacks and start the main loop
int main(int argc, char *argv[]) {
    std::string basepath = "./";
    if (argc > 2) {
        printf("Usage: %s [basepath]\n", argv[0]);
    } else if (argc == 2) {
        basepath = argv[1];
    }
    printf("Loading scene and shaders relative to path %s\n", basepath.c_str());

    // load scene data
    // parsing code is in objparser.cpp
    // take a look at the public interface in objparser.h
    if (!scene.parse(basepath + "data/sponza_low/sponza_norm.obj")) {
        return -1;
    }

    window = createOpenGLWindow(1024, 1024, "Assignment 5");

    // setup the event handlers
    // key handlers are defined in main.h
    // take a look at main.h to know what's in there.
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, motionCallback);

    glClearColor(0.8f, 0.8f, 1.0f, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    // TODO add loadXYZ() function calls here
    // loadTextures();
    // loadFramebuffer();

    camera.SetDimensions(600, 600);
    camera.SetPerspective(50);
    camera.SetDistance(10);
    camera.SetCenter(Vector3f(0, 1, 0));
    camera.SetRotation(Matrix4f::rotateY(1.6f) * Matrix4f::rotateZ(0.4f));

    // set timer for animations
    timer.set();
    while (!glfwWindowShouldClose(window)) {
        setViewportWindow(window);

        // we reload the shader files each frame.
        // this shaders can be edited while the program is running
        // loadPrograms/freePrograms is implemented in main.h
        bool valid_shaders = loadPrograms(basepath);
        if (valid_shaders) {

            // draw coordinate axes
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            if (gMousePressed) {
                drawAxis();
            }

            // update animation
            updateLightDirection();

            // draw everything
            draw();
        }
        // make sure to release the shader programs.
        freePrograms();

        // Make back buffer visible
        glfwSwapBuffers(window);

        // Check if any input happened during the last frame
        glfwPollEvents();
    } // END OF MAIN LOOP

    // All OpenGL resource that are created with
    // glGen* or glCreate* must be freed.
    // TODO: add freeXYZ() function calls here
    // freeFramebuffer();
    // freeTextures();

    glfwDestroyWindow(window);


    return 0;   // This line is never reached.
}
