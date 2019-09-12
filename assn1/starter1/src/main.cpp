#include "gl.h"
#include <GLFW/glfw3.h>

#include <cmath>
#include <iostream>
#include <cstdlib>
#include <fstream>
#include <vector>
#include <cstdint>

#include <vecmath.h>

#include "starter1_util.h"
#include "parse.h"
#include "curve.h"
#include "surf.h"
#include "camera.h"
#include "vertexrecorder.h"

using namespace std;

// If you're really interested in what "namespace" means, see
// Stroustup.  But basically, the functionality of putting all the
// globals in an "unnamed namespace" is to ensure that everything in
// here is only accessible to code in this file.
namespace
{
// Global variables here.

// This assignment uses a useful camera implementation
Camera camera;

// most curves are drawn with constant color, and no lighting
GLuint program_color;
// for surfaces, we apply a light+material shader
GLuint program_light;

// These are state variables for the UI
bool gMousePressed = false;
enum CurveMode {
   CURVE_MODE_NONE,
   CURVE_MODE_ACTIVE,
   CURVE_MODE_WITH_NORMALS
};
int  gCurveMode = CURVE_MODE_ACTIVE;
enum SurfaceMode {
   SURFACE_MODE_NONE,
   SURFACE_MODE_ACTIVE,
   SURFACE_MODE_WITH_NORMALS
};
int  gSurfaceMode = SURFACE_MODE_ACTIVE;
int  gPointMode = 1;

// This detemines how big to draw the normals
const float gLineLen = 0.1f;

// curve and surfaces vertices are recorded on application
// startup and reused when drawing each frame.
struct Recorders {
    VertexRecorder curve;
    VertexRecorder curveFrames;
    VertexRecorder surface;
    VertexRecorder surfaceNormals;
};
Recorders* recorders;

// These std::vectors store the control points, curves, and
// surfaces that will end up being drawn. Other std::vectors
// store the names for the curves and surfaces (as given by the files)
vector<vector<Vector3f> > gCtrlPoints;
vector<Curve> gCurves;
vector<string> gCurveNames;
vector<Surface> gSurfaces;
vector<string> gSurfaceNames;

// Declarations of functions whose implementations occur later.
void loadObjects(int argc, char *argv[]);
void initRendering();

void recordVertices();
void freeVertices();

void drawScene(void);
void drawAxis(void);
void drawCurve(void);
void drawPoints(void);
void drawSurface(void);


static void keyCallback(GLFWwindow* window, int key,
    int scancode, int action, int mods)
{
    if (action == GLFW_RELEASE) { // only handle PRESS and REPEAT
        return;
    }

    // Special keys (arrows, CTRL, ...) are documented
    // here: http://www.glfw.org/docs/latest/group__keys.html
    switch (key) {
    case GLFW_KEY_ESCAPE: // Escape key
        exit(0);
        break;
    case ' ':
    {
        Matrix4f eye = Matrix4f::identity();
        camera.SetRotation(eye);
        camera.SetCenter(Vector3f(0, 0, 0));
        break;
    }
    case 'c':
    case 'C':
        gCurveMode = (gCurveMode + 1) % 3;
        break;
    case 's':
    case 'S':
        gSurfaceMode = (gSurfaceMode + 1) % 3;
        break;
    case 'p':
    case 'P':
        gPointMode = (gPointMode + 1) % 2;
        break;
    default:
        cout << "Unhandled key press " << key << "." << endl;
    }
}

static void mouseCallback(GLFWwindow* window, int button, int action, int mods) {
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
    } else if (rstate == GLFW_PRESS) {
        gMousePressed = true;
        camera.MouseClick(Camera::RIGHT, x, y);
    } else if (mstate == GLFW_PRESS) {
        gMousePressed = true;
        camera.MouseClick(Camera::MIDDLE, x, y);
    } else {
        gMousePressed = true;
        camera.MouseRelease(x, y);
        gMousePressed = false;
    }
}

static void motionCallback(GLFWwindow* window, double x, double y)
{
    if (!gMousePressed) {
        return;
    }
    camera.MouseDrag((int)x, (int)y);
}

void setViewport(GLFWwindow* window)
{
    int w, h;
    glfwGetFramebufferSize(window, &w, &h);

    camera.SetDimensions(w, h);
    camera.SetViewport(0, 0, w, h);
    camera.ApplyViewport();
}

void drawAxis()
{
    glUseProgram(program_color);
    camera.SetUniforms(program_color);

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

void drawCurve()
{
    glUseProgram(program_color);
    camera.SetUniforms(program_color);

    glLineWidth(1);
    recorders->curve.draw(GL_LINES);
    if (gCurveMode == CURVE_MODE_WITH_NORMALS) {
        glLineWidth(1);
        recorders->curveFrames.draw(GL_LINES);
    }
}

void updateMaterialUniforms(GLuint program)
{
    GLfloat diffColor[] = { 0.4f, 0.4f, 0.4f, 1 };
    GLfloat specColor[] = { 0.9f, 0.9f, 0.9f, 1 };
    GLfloat shininess[] = { 50.0f };
    int loc = glGetUniformLocation(program, "diffColor");
    glUniform4fv(loc, 1, diffColor);
    loc = glGetUniformLocation(program, "specColor");
    glUniform4fv(loc, 1, specColor);
    loc = glGetUniformLocation(program, "shininess");
    glUniform1f(loc, shininess[0]);
}

void updateLightUniforms(GLuint program)
{
    GLfloat lightPos[] = { 3.0f, 3.0f, 5.0f, 1.0f };
    int loc = glGetUniformLocation(program, "lightPos");
    glUniform4fv(loc, 1, lightPos);

    GLfloat lightDiff[] = { 120.0f, 120.0f, 120.0f, 1.0f };
    loc = glGetUniformLocation(program, "lightDiff");
    glUniform4fv(loc, 1, lightDiff);
}

void drawSurface()
{
    const bool shaded = true; // TODO add UI for this variable
    if (shaded) {
        // DRAW SHADED SURFACE
        glUseProgram(program_light);
        camera.SetUniforms(program_light);
        updateMaterialUniforms(program_light);
        updateLightUniforms(program_light);


        // shade interior of polygons
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        // This tells openGL to *not* draw backwards-facing triangles.
        // This is more efficient, and in addition it will help you
        // make sure that your triangles are drawn in the right order.
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
    } else {
        // DRAW SURFACE WIRE FRAME
        glUseProgram(program_color);
        camera.SetUniforms(program_color);

        // don't shade polygon interior
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        glLineWidth(1);
    }
    recorders->surface.draw(GL_TRIANGLES);

    // DRAW SURFACE NORMALS
    if (gSurfaceMode == SURFACE_MODE_WITH_NORMALS) {
        glLineWidth(1);
        glUseProgram(program_color);
        camera.SetUniforms(program_color);
        recorders->surfaceNormals.draw(GL_LINES);
    }
}

void drawPoints()
{
    glUseProgram(program_color);
    camera.SetUniforms(program_color);

    // Setup for point drawing
    glPointSize(4);
    glLineWidth(1);

    glDisable(GL_DEPTH_TEST);
    const Vector3f COLOR(1, 1, 0.0f);
    for (int i = 0; i < (int)gCtrlPoints.size(); i++) {
        // There are relatively few control points, so we can
        // get away with recording this for each frame.
        VertexRecorder recorder;
        for (int j = 0; j < (int)gCtrlPoints[i].size(); j++) {
            recorder.record_poscolor(gCtrlPoints[i][j], COLOR);
        }
        recorder.draw(GL_POINTS);
        recorder.draw(GL_LINE_STRIP);
    }
    glEnable(GL_DEPTH_TEST);
}

void initRendering()
{
    // Clear to black
    glClearColor(0, 0, 0, 1);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

// Load in objects from standard input into the global variables: 
// gCtrlPoints, gCurves, gCurveNames, gSurfaces, gSurfaceNames.  If
// loading fails, this will exit the program.
void loadObjects(int argc, char *argv[])
{
    if (argc < 2) {
        cerr << "usage: " << argv[0] << " SWPFILE [OBJPREFIX] " << endl;
        exit(0);
    }

    ifstream in(argv[1]);
    if (!in) {
        cerr << argv[1] << " not found\a" << endl;
        exit(0);
    }


    cerr << endl << "*** loading and constructing curves and surfaces ***" << endl;

    if (!parseFile(in, gCtrlPoints,
        gCurves, gCurveNames,
        gSurfaces, gSurfaceNames)) {
        cerr << "\aerror in file format\a" << endl;
        in.close();
        exit(-1);
    }

    in.close();

    // This does OBJ file output
    if (argc > 2) {
        cerr << endl << "*** writing obj files ***" << endl;

        string prefix(argv[2]);

        for (int i = 0; i < (int)gSurfaceNames.size(); i++) {
            if (gSurfaceNames[i] != ".") {
                string filename =
                    prefix + string("_")
                    + gSurfaceNames[i]
                    + string(".obj");

                ofstream out(filename.c_str());

                if (!out) {
                    cerr << "\acould not open file " << filename << ", skipping" << endl;
                    out.close();
                    continue;
                } else {
                    outputObjFile(out, gSurfaces[i]);
                    cerr << "wrote " << filename << endl;
                }
            }
        }
    }
    cerr << endl << "*** done ***" << endl;
}

void recordVertices() {
    // For complex models, it is too expensive to specify
    // all vertices each frame. We can make things more efficient
    // by recording the vertices on application startup, and then
    // drawing from the pre-recorded data structure.

    recorders = new Recorders();

    // CURVES
    for (int i = 0; i < (int)gCurves.size(); i++) {
        recordCurve(gCurves[i], &recorders->curve);
    }

    for (int i = 0; i < (int)gCurves.size(); i++) {
        recordCurveFrames(gCurves[i], &recorders->curveFrames, gLineLen);
    }

    // SURFACE
    for (int i = 0; i < (int)gSurfaces.size(); i++) {
        recordSurface(gSurfaces[i], &recorders->surface);
    }
    for (int i = 0; i < (int)gSurfaces.size(); i++) {
        recordNormals(gSurfaces[i], &recorders->surfaceNormals, gLineLen);
    }
}

void freeVertices() {
    delete recorders;
    recorders = nullptr;
}

}
int main(int argc, char** argv)
{
    loadObjects(argc, argv);

    GLFWwindow* window = createOpenGLWindow(600, 600, "Assignment 1");

    // setup the event handlers
    glfwSetKeyCallback(window, keyCallback);
    glfwSetMouseButtonCallback(window, mouseCallback);
    glfwSetCursorPosCallback(window, motionCallback);

    initRendering();

    // The program object controls the programmable parts
    // of OpenGL. All OpenGL programs define a vertex shader
    // and a fragment shader.
    program_light = compileProgram(c_vertexshader, c_fragmentshader_light);
    if (!program_light) {
        printf("Cannot compile program\n");
        return -1;
    }
    program_color = compileProgram(c_vertexshader, c_fragmentshader_color);
    if (!program_color) {
        printf("Cannot compile program\n");
        return -1;
    }

    camera.SetDimensions(600, 600);
    camera.SetPerspective(50);
    camera.SetDistance(10);
    camera.SetCenter(Vector3f(0, 0, 0));

    recordVertices();
    // Main Loop
    while (!glfwWindowShouldClose(window)) {
        // Clear the rendering window
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        setViewport(window);

        if (gMousePressed) {
            drawAxis();
        }
        if (gCurveMode) {
            drawCurve();
        }
        if (gSurfaceMode) {
            drawSurface();
        }
        if (gPointMode) {
            drawPoints();
        }

        // Make back buffer visible
        glfwSwapBuffers(window);

        // Check if any input happened during the last frame
        glfwPollEvents();
    }

    // All OpenGL resource that are created with
    // glGen* or glCreate* must be freed.
    freeVertices();
    glDeleteProgram(program_color);
    glDeleteProgram(program_light);

    glfwTerminate(); // destroy the window
    return 0;
}
