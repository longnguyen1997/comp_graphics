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

std::map<std::string, GLuint> gltextures;

GLuint fb, fb_depthtex, fb_colortex;


// FUNCTION IMPLEMENTATIONS

Matrix4f getLightView() {
    // center should be the center of the sponze scene
    // (i.e. position of the dragon statue).
    Vector3f center = Vector3f(0, 1, 0);
    // The line of sight from eye to center
    // should be parallel to light dir.
    Vector3f eye = center + light_dir;
    // Up must be orthogonal to light dir.
    Vector3f up = Vector3f(-light_dir.y(),
                           light_dir.x(),
                           0).normalized();
    return Matrix4f::lookAt(eye, center, up);
}

Matrix4f getLightProjection() {
    int scale = 50;
    int clip = 10;
    return Matrix4f::orthographicProjection(scale, scale, -clip, clip);
}

void loadFramebuffer() {

    // COLOR
    // Inside loadFramebuffer(), request a valid handle
    // for each texture with calls to glGenTextures().
    glGenTextures(1, &fb_colortex);
    // Bind the color texture to GL TEXTURE 2D.
    glBindTexture(GL_TEXTURE_2D, fb_colortex);
    // Use glTexImage2D to allocate space for it.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // DEPTH
    glGenTextures(1, &fb_depthtex);
    glBindTexture(GL_TEXTURE_2D, fb_depthtex);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    // FRAMEBUFFER
    glGenFramebuffers(1, &fb);
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                           GL_TEXTURE_2D, fb_colortex, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,
                           GL_TEXTURE_2D, fb_depthtex, 0);

    // Check that things went well.
    GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if ( status != GL_FRAMEBUFFER_COMPLETE) {
        printf("ERROR, incomplete framebuffer\n");
        exit(1);
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

}

void freeFramebuffer() {
    glDeleteFramebuffers(1, &fb);
    glDeleteTextures(1, &fb_depthtex);
    glDeleteTextures(1, &fb_colortex);
}

void loadTextures() {
    for (auto it = scene.textures.begin(); it != scene.textures.end(); ++it) {
        std::string name = it->first;
        rgbimage &im = it->second;

        // Create OpenGL Texture
        GLuint gltexture;
        glGenTextures(1, &gltexture);
        glBindTexture(GL_TEXTURE_2D, gltexture);
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8,
                     im.w, im.h, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, im.data.data());
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gltextures.insert(std::make_pair(name, gltexture));
    }
}

void freeTextures() {
    for (auto it = scene.textures.begin(); it != scene.textures.end(); ++it) {
        glDeleteTextures(1, &gltextures[it->first]);
    }
}

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
        /*
        Back in the drawScene function, you must make sure to bind
        the right texture before calling recorder.draw(). Just
        before you draw (1) look up the needed texture handle
        from the std::map, and (2) glBindTexture() it as GL TEXTURE 2D.
        */
        glBindTexture(GL_TEXTURE_2D, gltextures[batch.mat.diffuse_texture]);
        // For shadow mapping, after setting up the diffuse texture
        // in drawScene, we switch to texture unit 1, bind the depth
        // texture, and then switch back to texture unit 0 to make
        // sure we don’t affect other code that expects unit 0
        // (the default) to be active.
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, fb_depthtex);
        glActiveTexture(GL_TEXTURE0);
        int loc = glGetUniformLocation(program, "shadowTex");
        glUniform1i(loc, 1);
        recorder.draw();
    }
}

void draw() {
    // 2. DEPTH PASS
    // - bind framebuffer
    glBindFramebuffer(GL_FRAMEBUFFER, fb);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // - configure viewport
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glUseProgram(program_color);
    // - compute camera matrices (light source as camera)
    Matrix4f LV = getLightView();
    Matrix4f P = getLightProjection();
    // - call drawScene
    drawScene(program_color, LV, P);

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
    glViewport(0, 0, 256, 256);
    drawTexturedQuad(fb_depthtex);
    glViewport(256, 0, 256, 256);
    drawTexturedQuad(fb_colortex);
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
    loadTextures();
    loadFramebuffer();

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
    freeFramebuffer();
    freeTextures();

    glfwDestroyWindow(window);


    return 0;   // This line is never reached.
}
