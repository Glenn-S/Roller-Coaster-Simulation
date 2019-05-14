/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Some of the functions in this class were borrowed and modified from
 * the boilerplate code provided by Andrew Owens in CPSC 587.
 */

#ifndef GRAPHICSPROGRAM_H
#define GRAPHICSPROGRAM_H

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <irrKlang.h>

#include "camera.h"
#include "curve.h"
#include "curvefileio.h"
#include "mat4f.h"
#include "openglmatrix.h"
#include "program.h"
#include "vec3f.h"

#include "Geometry.h"
#include "RenderingEngine.h"
#include "CoasterPhysics.h"

using namespace opengl;
using namespace std;
using namespace irrklang;

class GLFWwindow;

enum ANGLE {CAR = 0, DISTANT = 1, TRACKING = 2};

class GraphicsProgram {
public:

    GraphicsProgram(string title);
    virtual ~GraphicsProgram();

    void start();
    void setupWindow();
    void systemInfo();
    void cleanup();

    void displayFunc();
    void resizeFunc();

    bool init();

    void setupScene(vector<Geometry*> graph);
    void deleteScene(vector<Geometry*> graph);
    bool loadInTrack();
    void loadInGeometry();
    bool loadMeshGeometryToGPU();
    bool loadCurveGeometryToGPU();

    void reloadProjectionMatrix();
    void reloadViewMatrix();

    void animate(int t);
    void oncePerFrame();
    void updateTrain(unsigned int vertexID);
    void simulationStep(int t);

    void moveCamera();
    void resetCamera(openGL::scene::Camera &camera);



/***************************** GLOBAL VARS ***************************************/
    RenderingEngine *renderer; // rendering engine reference

    // AUDIO PLAYER AND ATTR
    ISoundEngine *mediaPlayer;
    ISoundSource *liftSFX;
    ISoundSource *roarSFX;
    ISound *liftAudio, *roarAudio;
    bool liftAudioPlaying = false, roarAudioPlaying = false;


    // DRAWING PROGRAMS MANAGER
    vector<opengl::Program> g_program; // holds shader programs


    // SCENE GEOMETRY
    Geometry g_car1Data, g_car2Data, g_car3Data;

///////////////////////////////////////////////////////
    const unsigned int carDistance = 350; // indices
//////// CHANGE ///////////////////////////////////////

    Geometry g_floorData, g_gateData, g_trackData, g_supportsData;
    math::Vec3f cartColour = math::Vec3f(0.5, 0.5, 0.2);
    math::Vec3f trackColour = math::Vec3f(1, 0, 0);
    math::Vec3f supportsColour = math::Vec3f(1, 0, 0);
    math::Vec3f groundColour = math::Vec3f(0.177, 0.341, 0.173);
    math::Vec3f gateColour = math::Vec3f(0.71, 0.396, 0.114); // light brown
    vector<Geometry*> sceneGraph; // to store all of the geometry for iterating through


    // BACKGROUND COLOUR
    math::Vec3f BACKGROUND = math::Vec3f(0.529, 0.808, 0.922);


    // TRAIN PARAMETERS
    const double TIME = 0.015f; // delta t steps in seconds (I made my steps larger)
    uint32_t curveVertexID = 0; // global index storage (arbitrary start point)


    // CURVE GEOMETRY
    Geometry g_curveData;
    string g_curveFilePath = "./curves/rollerCoaster.obj";
    math::geometry::Curve g_curve; // data structure for storing curve points

    const int32_t g_numberOfSubdivisions = 19; // number of subdivisions for defining curve smoothness


    // LIGHTING
    math::Vec3f LIGHT_SOURCE = math::Vec3f(20.0, 40.0, 0.0);


    // MPV MATRICES
    math::Mat4f g_V; // view matrix
    math::Mat4f g_P; // projection matrix
    math::Mat4f g_MVP; // model, view, projection matrix


    // CAMERA AND ATTRIBUTES
    math::Vec3f CAM_POS = math::Vec3f(12.0, 12.0, 12.0);
    openGL::scene::Camera CAM_DEFAULT;
    openGL::scene::Camera g_camera; // scene camera
    openGL::scene::CameraUpdate g_cameraUpdate; // camaera update struct
    ANGLE CAMERA_ANGLE = DISTANT;


    // CAMERA PROPERTIES
    float g_rotationSpeed = 1.0f;
    float g_panningSpeed = 0.25f;
    float g_cursorSpeed = 0.05f;
    bool g_cursorLocked;
    float g_cursorX, g_cursorY;


    // FRAME PLAY
    bool g_play = false;


    // WINDOW PROPERTIES
    int WIN_WIDTH = 800, WIN_HEIGHT = 600;
    int FB_WIDTH = 800, FB_HEIGHT = 600;
    float WIN_FOV = 50.f;
    float WIN_NEAR = 0.01f;
    float WIN_FAR = 100.f;

private:
    GLFWwindow *window; // window pointer reference
    string TITLE; // window title
};



/************************************** FREE FUNCTIONS ***********************************************/

openGL::scene::Camera glLookAtCamera(math::Vec3f cam,
                                     math::Vec3f lookAtObj,
                                     math::Vec3f worldUp);

// Callback Functions
void ErrorCallback(int error, const char* description);
void windowSetSizeFunc();
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowSetSizeFunc(GLFWwindow *window, int width, int height);
void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height);
void windowMouseButtonFunc(GLFWwindow *window, int button, int action, int mods);
void windowMouseMotionFunc(GLFWwindow *window, double x, double y);
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods);

std::string GL_ERROR();


#endif // GRAPHICSPROGRAM_H
