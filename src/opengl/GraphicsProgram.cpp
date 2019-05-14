/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Some of the functions in this class were borrowed and modified from
 * the boilerplate code provided by Andrew Owens in CPSC 587 and the
 * structure style is a mix between Andrews boilerplate style and
 * the code style borrowed from John Hall's boilerplate code from
 * CPSC 453.
 */

#include <cmath>
#include <iostream>
#include <limits>
#include <vector>
#include <ctime>

// SOUND LIBRARY
#include <irrKlang.h>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "camera.h"
#include "curve.h"
#include "curvefileio.h"
#include "mat4f.h"
#include "openglmatrix.h"
#include "program.h"
#include "vec3f.h"
#include "Geometry.h"
#include "RenderingEngine.h"
#include "GraphicsProgram.h"
#include "CoasterPhysics.h"
#include "Model.h"




// MACROS
#define DEBUG 0 // debug mode = 1
#define SOUND_ENABLE 1 // enable sound 1, turn off 0




using namespace opengl;
using namespace std;
using namespace math::physics;
using namespace irrklang;

GraphicsProgram *prog; // global pointer to this program


/**************************** CONSTRUCTOR/DESTRUCTOR *********************************/
GraphicsProgram::GraphicsProgram(string title) : TITLE(title) {
    prog = this; // initalize global pointer to this instance
    setupWindow();
    renderer = new RenderingEngine();

#if SOUND_ENABLE
    mediaPlayer = createIrrKlangDevice(); // setup the media player
    if (!mediaPlayer)
        exit(EXIT_FAILURE);
    // load sound sources
    liftSFX = mediaPlayer->addSoundSourceFromFile("./sounds/lift.wav");
    roarSFX = mediaPlayer->addSoundSourceFromFile("./sounds/roar.wav");
    liftAudio = mediaPlayer->play2D(liftSFX, false, true, true); // starts paused and can be tracked
    roarAudio = mediaPlayer->play2D(roarSFX, true, true, true);
    mediaPlayer->update();
    mediaPlayer->setSoundVolume(0.5); // set the overall volume to half the max

    if (!liftAudio && !roarAudio) {
        cerr << "Sound player failed to initialize" << endl;
        exit(EXIT_FAILURE);
    }
#endif

}

GraphicsProgram::~GraphicsProgram() {
    delete renderer;

#if SOUND_ENABLE
    // clean up media memory
    mediaPlayer->removeAllSoundSources();
    liftSFX->drop();
    roarSFX->drop();
    mediaPlayer->drop();
#endif

}

/********************************* PROGRAM FUNCTIONS ********************************/
/**
 * Main program loop that runs every frame to generate the animations
 */
void GraphicsProgram::start() {

    if (init()) { // Initialize all the geometry, and load it once to the GPU
        clock_t current = std::clock(), now; // frame rate timer

        while (glfwGetKey(window, GLFW_KEY_ESCAPE) != GLFW_PRESS && !glfwWindowShouldClose(window)) {
            now = std::clock();

            if ((now - current) / (double)CLOCKS_PER_SEC >= 1.0/60.0) {
                current = now; // update time

                if (g_play)
                    oncePerFrame(); // only perform if the program isn't paused

                displayFunc(); // send result to GPU to display
                moveCamera(); // update the camera matrix

                glfwSwapBuffers(window);
                glfwPollEvents();
            }
        }
    }
    cleanup(); // clean up memory
    return;
}

/**
 * Sets up the window context parameters for openGL
 *
 * Borrowed and modified from boilerplate code provided by Andrew Owens.
 */
void GraphicsProgram::setupWindow() {
    if (!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //  glfwWindowHint(GLFW_SAMPLES, 4);

    const GLFWvidmode *mode = glfwGetVideoMode(glfwGetPrimaryMonitor());
    glfwWindowHint(GLFW_RED_BITS, mode->redBits);
    glfwWindowHint(GLFW_GREEN_BITS, mode->greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, mode->blueBits);
    glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);

    window = glfwCreateWindow(WIN_WIDTH, WIN_HEIGHT, TITLE.c_str(), NULL, NULL);
    if (!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    //Errors will be printed to the console
    glfwSetErrorCallback(ErrorCallback);

    glfwGetWindowSize(window, &WIN_WIDTH, &WIN_HEIGHT);
    glfwGetFramebufferSize(window, &FB_WIDTH, &FB_HEIGHT);

    glfwSetWindowSizeCallback(window, windowSetSizeFunc);
    glfwSetFramebufferSizeCallback(window, windowSetFramebufferSizeFunc);
    glfwSetKeyCallback(window, windowKeyFunc);
    glfwSetCursorPosCallback(window, windowMouseMotionFunc);
    glfwSetMouseButtonCallback(window, windowMouseButtonFunc);

    glfwMakeContextCurrent(window);
    gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    glfwSwapInterval(1); // vsync 1 for on, 0 for off

    systemInfo();
}

/**
 * Get the system information and print to prompt.
 *
 * Borrowed from John Hall's boilerplate code from CPSC 453
 */
void GraphicsProgram::systemInfo() {
    // query opengl version and renderer information
    string version = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    string glslver = reinterpret_cast<const char *>(glGetString(GL_SHADING_LANGUAGE_VERSION));
    string renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));

    cout << "OpenGL: " << version << endl
         << "GLSL Version: " << glslver << endl
         << "Renderer: " << renderer << endl;
}




/*********************************** PROGRAM SETUP **************************************/

/**
 * Enables the depth paramaters and sets up the GPU VAO's and VBO's. Once setup, it also
 * loads in the geometry to be used
 */
bool GraphicsProgram::init() {
    using namespace math::geometry;

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glEnable(GL_MULTISAMPLE);
    glPointSize(50);
    glEnable(GL_LINE_SMOOTH);

    // SETUP SHADERS, BUFFERS, VAOs
    if (!loadInTrack())
        return false;

    // get the scene elements loaded in with their properties
    loadInGeometry();

    // set up the buffers for the GPU
    if (!renderer->reloadShadersFromFile(g_program))
        return false;
    setupScene(sceneGraph);

    // load cart and track triangles into GPU
    if (!loadMeshGeometryToGPU()) return false;
    if (!loadCurveGeometryToGPU()) return false;

    resetCamera(g_camera); // get the camera ready

    // set the starting position for the camera
    CAM_DEFAULT = glLookAtCamera(CAM_POS, math::Vec3f(), math::Vec3f(0.0, 1.0, 0.0));
    g_camera = CAM_DEFAULT;
    reloadProjectionMatrix();
    reloadViewMatrix();

    return true;
}

/**
 * read in the track from config file and reparamaterize it
 *
 * borrowed from Andrew Owens boilerplate code.
 */
bool GraphicsProgram::loadInTrack() {
    using namespace math::geometry;

    // get the track info and load it in to memory
    if (g_curveFilePath.empty()) {
        return false;
    } else {
        // auto curve = loadCurveFromFile(g_curveFilePath); // load from .txt file
        auto curve = loadCurveFrom_OBJ_File(g_curveFilePath); // load from .obj file
        if (curve.pointCount() == 0) {
            cerr << "curve is empty" << endl;
            return false;
        }
        g_curve = Curve(move(curve)); // load curve data into global curve variable
    }

    // reparameterize the curve
    g_curve = math::geometry::cubicSubdivideCurve(g_curve, g_numberOfSubdivisions);
    g_curve = ttlArcLengthReParam(g_curve, (unsigned int)(length(g_curve) * 1000));
    curveVertexID = LIFT_START; // set the starting position for the roller coaster simulation

#if DEBUG
    cout << "start: " << LIFT_START << ", decel: " << DECEL_START << endl;
#endif
    return true;
}


/**
 * retrieve all of the models and set their parameters and store them
 * into the scene graph.
 */
void GraphicsProgram::loadInGeometry() {
    // store all the scene data structures in an array
    sceneGraph.push_back(&g_trackData);
    sceneGraph.push_back(&g_supportsData);
    sceneGraph.push_back(&g_floorData);
    sceneGraph.push_back(&g_gateData);
    sceneGraph.push_back(&g_car1Data);
    sceneGraph.push_back(&g_car2Data);
    sceneGraph.push_back(&g_car3Data);

    // read in models
    scene::Model::modelParser(g_car1Data, "./models/coasterCar.obj");
    g_car2Data = g_car1Data;
    g_car3Data = g_car1Data;
    scene::Model::modelParser(g_floorData, "./models/floor.obj");
    scene::Model::modelParser(g_gateData, "./models/gate.obj");
    g_gateData.modelMatrix = openGL::TranslateMatrix(math::Vec3f(4, 0, 2.5)) * openGL::UniformScaleMatrix(0.2f);
    generateTrack(g_curve, g_trackData, TIME);
    generateSupports(g_curve, g_supportsData, TIME);

    // set the draw modes
    g_trackData.drawMode = GL_TRIANGLE_STRIP;
    g_supportsData.drawMode = GL_LINES;
    g_floorData.drawMode = GL_TRIANGLE_STRIP;
    g_gateData.drawMode = GL_TRIANGLES;

    g_car1Data.drawMode = GL_TRIANGLES;
    g_car2Data.drawMode = GL_TRIANGLES;
    g_car3Data.drawMode = GL_TRIANGLES;

    // set the polygon mesh modes
    g_trackData.polygonMode = GL_LINE;
    g_supportsData.polygonMode = GL_LINE;
    g_floorData.polygonMode = GL_FILL;
    g_gateData.polygonMode = GL_FILL;

    g_car1Data.polygonMode = GL_FILL;
    g_car2Data.polygonMode = GL_FILL;
    g_car3Data.polygonMode = GL_FILL;

    // set the colours
    g_trackData.colour = trackColour;
    g_supportsData.colour = supportsColour;
    g_floorData.colour = groundColour;
    g_gateData.colour = gateColour;

    g_car1Data.colour = cartColour;
    g_car2Data.colour = cartColour;
    g_car3Data.colour = cartColour;

    updateTrain(curveVertexID);
}


/**
 * Clean up after the program has finished by cleaning up memory.
 */
void GraphicsProgram::cleanup() {
    deleteScene(sceneGraph);
    g_program.clear(); // calls destructors on shaders, deallocates GPU
    glfwDestroyWindow(window);
    glfwTerminate();
}


/**
 * Go through the scene graph and setup all of the buffer ID's
 */
void GraphicsProgram::setupScene(vector<Geometry*> graph) {
    for (Geometry *g : graph) {
        if (g->children.size() > 0)
            setupScene(g->children); // recurse through the scene graph
        renderer->assignBuffer(*g);
        renderer->setBufferData(*g);
    }
}

/**
 * traverse the scene graph deleting all buffers assigned
 */
void GraphicsProgram::deleteScene(vector<Geometry*> graph) {
    // clean up geometry
    for (Geometry *g : graph) {
        if (g->children.size() > 0)
            deleteScene(g->children);
        renderer->deleteBuffer(*g);
    }
}


/******************************* BIND BUFFER DATA TO GPU *************************************/

/**
 * To go through all of the scenes geometry and bind their buffer ID's
 * to the GPU.
 *
 * Borrowed from boilerplate code from Andrew Owens in CPSC 587.
 */
bool GraphicsProgram::loadMeshGeometryToGPU() {

    // load in all of the geometry for meshes
    for (Geometry *g : sceneGraph) {

        // load all the colours into the roller coaster parts
        for (unsigned int i = 0; i < g->verts.size(); i++) {
            g->colours.push_back(g->colour);
        }

        // load vertices
        glBindBuffer(GL_ARRAY_BUFFER, g->vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(math::Vec3f) * g->verts.size(), // byte size of Vec3f
                     g->verts.data(),    // pointer (Vec3f*) to contents of verts
                     GL_STATIC_DRAW); // Usage pattern of GPU buffer

        // load normals
        glBindBuffer(GL_ARRAY_BUFFER, g->normalBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(math::Vec3f) * g->normals.size(), // byte size of Vec3f
                     g->normals.data(),    // pointer (Vec3f*) to contents of verts
                     GL_STATIC_DRAW); // Usage pattern of GPU buffer

        // load colours
        glBindBuffer(GL_ARRAY_BUFFER, g->colourBufferID);
        glBufferData(GL_ARRAY_BUFFER,
                     sizeof(math::Vec3f) * g->colours.size(), // byte size of Vec3f
                     g->colours.data(),    // pointer (Vec3f*) to contents of verts
                     GL_STATIC_DRAW); // Usage pattern of GPU buffer
    }

    return true;
}

/**
 * Update the GPU curve data being stored, used now for debugging
 *
 * This was borrowed from the boilerplate code provided by Andrew Owens.
 */
bool GraphicsProgram::loadCurveGeometryToGPU() {
    using namespace math::geometry;

    // load curve
    glBindBuffer(GL_ARRAY_BUFFER, g_curveData.vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER,
                 sizeof(math::Vec3f) * g_curve.pointCount(), // byte size of Vec3f, 4 of them
                 g_curve.data(),  // pointer (Vec3f*) to contents of verts
                 GL_STATIC_DRAW); // Usage pattern of GPU buffer
    g_curveData.verticesCount = g_curve.pointCount();

    return true;
}







/********************************** CALLED EVERY FRAME **********************************/
/**
 * Send vertices to the GPU to be passed through the graphics pipeline.
 *
 * Borrowed and modified from Andrew Owens boilerplate code provided.
 */
void GraphicsProgram::displayFunc() {
    glClearColor(BACKGROUND.m_x, BACKGROUND.m_y, BACKGROUND.m_z, 1.0f); // set background colour
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    auto &program = g_program[0]; // select the shading program to use
    program.use();

    // draw each piece of geoemtry
    for (Geometry *g : sceneGraph) {
        glPolygonMode( GL_FRONT_AND_BACK, g->polygonMode );
        g_MVP = g_P * g_V * g->modelMatrix;
        program.setUniformMat4f("MVP", g_MVP, true); // true, transpose for stupid OpenGL
        program.setUniformMat4f("M", g->modelMatrix, true); // send the model matrix
        program.setUniformVec3f("COLOUR", g->colour);
        program.setUniform1i("shade", 1);

        glBindVertexArray(g->vaoID);
        glDrawArrays(g->drawMode, 0, g->verts.size());
    }

    program.setUniformVec3f("lightPosition_worldSpace", LIGHT_SOURCE); // light
    program.setUniformVec3f("cameraPosition_worldSpace", g_camera.localPos()); // camera

}








/************************************ CONTROL ANIMATION ***********************************/

/**
 * Move along the curve incrementing based on velocity of point. Also update
 * the audio if need be.
 */
void GraphicsProgram::oncePerFrame() {
    // update for next frame
    unsigned int tolerance = 2;
    if ( (curveVertexID >= LIFT_START - tolerance) && (curveVertexID <= LIFT_START) ) // check the end tolerance
        curveVertexID = LIFT_START; // ensure that the train always starts at exactly the LIFT_START

    double speed = v(g_curve, curveVertexID);
    curveVertexID = getPosition(g_curve, curveVertexID, speed, TIME);
    curveVertexID = curveVertexID % g_curve.pointCount(); // wrap around

#if DEBUG
    cout << "CURVE ID: " << curveVertexID << endl;
#endif

#if SOUND_ENABLE
    // update audio
    unsigned int max = getMaxIndex(g_curve);

    if (curveVertexID >= LIFT_START || curveVertexID < max + 500) {
        // if this is called g_play must be true
        if (!liftAudioPlaying) {
            liftAudio->setVolume(0.4);
            liftAudioPlaying = true;
            liftAudio->setIsPaused(false); // play the music
            roarAudioPlaying = false;
            roarAudio->setIsPaused(true);
        }
        if ( (curveVertexID >= max) && (curveVertexID < max + 500) ) {
            double volume = 1 - ((double)(curveVertexID - (max)) / (double)500);
            liftAudio->setVolume(liftAudio->getVolume() * volume);
        }
    } else if (curveVertexID >= max && curveVertexID < DECEL_START + 2000) {
        if (liftAudioPlaying) { // turn off lift effects
            liftAudioPlaying = false;
            liftAudio->setIsPaused(true); // at the top of the lift
        } else {
            if (!roarAudioPlaying) {
                roarAudio->setVolume(0.4);
                roarAudioPlaying = true;
                roarAudio->setIsPaused(false);
                roarAudio->setIsLooped(false);
            }
        }
        if ( (curveVertexID >= DECEL_START) && (curveVertexID <= DECEL_START + 2000) ) {
            double volume = 1 - ((double)(curveVertexID - (DECEL_START)) / (double)2000);
            roarAudio->setVolume(roarAudio->getVolume() * volume);
        }
    } else { // at the end so pause the sound
        liftAudioPlaying = false;
        liftAudio->setIsPaused(true); // at the top of the lift
        liftAudio->setPlayPosition(0); // reset
        roarAudioPlaying = false;
        roarAudio->setIsPaused(true);
        roarAudio->setPlayPosition(0);
    }
#endif

    animate(curveVertexID);
}

/**
 * Retrieve the coordinates of the vertexID and update the objects modelMatrix
 * translation and scaling. Mostly a wrapper function now and in place for if other
 * objects were to be added.
 */
void GraphicsProgram::animate(int vertexID) {
    updateTrain(vertexID);
}


/**
 * update the position of each car based on the middle car which
 * is the center of gravity for this train.
 */
void GraphicsProgram::updateTrain(unsigned int vertexID) {
    using namespace openGL;

    // get the carts orientation and add it to the model
    int newID;
    math::Vec3f pos = g_curve[vertexID]; // retrieve the position in the curve matrix
    math::Mat4f RotationMatrix = getOrientation(g_curve, vertexID, TIME);
    g_car1Data.modelMatrix = TranslateMatrix(pos) * RotationMatrix * UniformScaleMatrix(0.1f);

    // car 3 (front car)
    newID = (vertexID + carDistance) % g_curve.pointCount();
    pos = g_curve[newID];
    RotationMatrix = getOrientation(g_curve, newID, TIME);
    g_car2Data.modelMatrix = openGL::TranslateMatrix(pos) * RotationMatrix * UniformScaleMatrix(0.1f);

    // car 4 (back car)
    newID = (vertexID - carDistance);
    if (newID < 0)
        newID = g_curve.pointCount() + newID; // loop to the back
    pos = g_curve[newID];
    RotationMatrix = getOrientation(g_curve, newID, TIME);
    g_car3Data.modelMatrix = openGL::TranslateMatrix(pos) * RotationMatrix * UniformScaleMatrix(0.1f);

}






//////////////////////////////////////////////////////////////////////////////////////////////

/**
 * Reloads the projection matrix using the window scene attributes
 *
 * This was borrowed from Andrew Owens from the boilerplate code provided
 * in CPSC 587.
 */
void GraphicsProgram::reloadProjectionMatrix() {
    g_P = openGL::PerspectiveProjection(WIN_FOV, // FOV
                                        static_cast<float>(WIN_WIDTH) / WIN_HEIGHT, // Aspect
                                        WIN_NEAR,       // near plane
                                        WIN_FAR);       // far plane depth
}

/**
 * Resets the view matrix using the camera as a model
 *
 * This was borrowed from Andrew Owens from the boilerplate code provided
 * in CPSC 587.
 */
void GraphicsProgram::reloadViewMatrix() { g_V = openGL::scene::makeViewMatrix(g_camera); }

/**
 * Updates camera if update bits are set or if the camera is in CAR or
 * TRACKING mode
 *
 * This code was borrowed from Andrew Owens from the boilerplate
 * code provided in CPSC 587 and modified by Glenn Skelton.
 */
void GraphicsProgram::moveCamera() {
    using namespace openGL::scene;
    math::Vec3f normal, tangent, binormal;

    // change the camera type according to
    switch (CAMERA_ANGLE) {
    case CAR:
        // update the camera
        tangent = getTangent(g_curve, curveVertexID, TIME);
        normal = getNormal(g_curve, curveVertexID, TIME);
        binormal = normalized(cross(tangent, normal));
        normal = normalized(cross(binormal, tangent));

        g_camera = openGL::scene::Camera(g_curve[curveVertexID] + normal*0.2, // position
                                         tangent, // forward
                                         normal); // up
        reloadViewMatrix();
        break;

    case DISTANT: // allow regular user interactions for moving the camera
        if (g_cameraUpdate.needsUpdating()) {
            if (g_cameraUpdate.isSet(CameraUpdate::moveBackward)) {
                g_camera.moveBackward(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::moveForward)) {
                g_camera.moveForward(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::moveUp)) {
                g_camera.moveUp(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::moveDown)) {
                g_camera.moveDown(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::moveLeft)) {
                g_camera.moveLeft(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::moveRight)) {
                g_camera.moveRight(g_panningSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rotateLeft)) {
                g_camera.rotateLeft(g_rotationSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rotateRight)) {
                g_camera.rotateRight(g_rotationSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rotateUp)) {
                g_camera.rotateUp(g_rotationSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rotateDown)) {
                g_camera.rotateDown(g_rotationSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rollLeft)) {
                g_camera.rollLeft(g_rotationSpeed);
            }
            if (g_cameraUpdate.isSet(CameraUpdate::rollRight)) {
                g_camera.rollRight(g_rotationSpeed);
            }

            reloadViewMatrix();
            // g_cameraUpdate.reset(); // reseting seems jittery, so don't
        }
        break;

    case TRACKING:
        // define points along the track to set the camera to follow the cart
        unsigned int cam1 = 172000,
                     cam2 = getMaxIndex(g_curve)-500,
                     cam3 = 16000,
                     cam4 = 34523,
                     cam5 = 84000,
                     cam6 = 145000;

        math::Vec3f camPos;
        math::Vec3f cartPos = g_curve[curveVertexID];
        math::Vec3f worldUp = math::Vec3f(0, 1.0, 0);

        // change camera depending on where the train is on the track
        if (curveVertexID >= cam1 || curveVertexID < cam2) {
            camPos = math::Vec3f(-10.0, 5.0, 10.0);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        } else if (curveVertexID >= cam2 && curveVertexID < cam3) {
            camPos = math::Vec3f(-7.0, 6.0, 0.5);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        } else if (curveVertexID >= cam3 && curveVertexID < cam4) {
            camPos = math::Vec3f(0.0, 2.0, -6.5);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        } else if (curveVertexID >= cam4 && curveVertexID < cam5) {
            camPos = math::Vec3f(-1.0, 1.0, 3.0);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        } else if (curveVertexID >= cam5 && curveVertexID < cam6) {
            camPos = math::Vec3f(-5.0, 0.1, 0.0);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        } else if (curveVertexID >= cam6 && curveVertexID < cam1) {
            camPos = math::Vec3f(3.0, 1.0, 0.0);
            g_camera = glLookAtCamera(camPos, cartPos, worldUp);
        }

        reloadViewMatrix();
        break;
    }

}




//////////////////////////////////////////////////////////////////////////////////////////////

// FREE FUNCTIONS

/**
 * To act like the lookAt() function provided by glm for setting the cameras orientation
 */
openGL::scene::Camera glLookAtCamera(math::Vec3f cam, math::Vec3f lookAtObj, math::Vec3f worldUp) {
    math::Vec3f lookAt = math::normalized(lookAtObj - cam);
    math::Vec3f camBinormal = math::cross(lookAt, worldUp);
    math::Vec3f up = math::cross(camBinormal, lookAt);
    return openGL::scene::Camera(cam, lookAt, up);
}

//==================== CALLBACK FUNCTIONS ====================//

/**
 * To return the GLFW error code
 *
 * This was borrowed from the boilerplate code provided by
 * John Hall in CPSC 453.
 */
void ErrorCallback(int error, const char* description) {
    cout << "GLFW ERROR " << error << ":" << endl;
    cout << description << endl;
}

/**
 * To adjust the projection matrix if the screen sizing has changed
 *
 * This was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587.
 */
void windowSetSizeFunc(GLFWwindow *window, int width, int height) {
    prog->WIN_WIDTH = width;
    prog->WIN_HEIGHT = height;

    prog->reloadProjectionMatrix();
}

/**
 * To get the window frame buffer size and adjust the view port based
 * on a screen size change.
 *
 * This code was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587.
 */
void windowSetFramebufferSizeFunc(GLFWwindow *window, int width, int height) {
    prog->FB_WIDTH = width;
    prog->FB_HEIGHT = height;

    glViewport(0, 0, prog->FB_WIDTH, prog->FB_HEIGHT);
}

/**
 * To read if a mouse button action has occured.
 *
 * This code was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587 and modified by Glenn Skelton.
 */
void windowMouseButtonFunc(GLFWwindow *window, int button, int action, int mods) {
    if (prog->CAMERA_ANGLE != CAR) { // as long as we are not in first person, allow modification
        if (button == GLFW_MOUSE_BUTTON_LEFT) {
            if (action == GLFW_PRESS) {
                prog->g_cursorLocked = GL_TRUE;
            } else {
                prog->g_cursorLocked = GL_FALSE;
            }
        }
    }
    // else it is disabled
}

/**
 * To get the position of the mouse and return mouses position
 *
 * This was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587.
 */
void windowMouseMotionFunc(GLFWwindow *window, double x, double y) {
    if (prog->CAMERA_ANGLE == DISTANT) {
        if (prog->g_cursorLocked) {
            float deltaX = (x - prog->g_cursorX) * prog->g_cursorSpeed;
            float deltaY = (y - prog->g_cursorY) * prog->g_cursorSpeed;
            prog->g_camera.rotateRightAroundFocus(deltaX);
            prog->g_camera.rotateDownAroundFocus(deltaY);
            prog->reloadViewMatrix();
        }
    }
    // else it is disabled

    prog->g_cursorX = x;
    prog->g_cursorY = y;
}


/**
 * To get the key input and modify the program based on which key was pressed
 *
 * This was borrowed from Andrew Owens from the boilerplate code provided in
 * CPSC 587 and was modified by Glenn Skelton.
 */
void windowKeyFunc(GLFWwindow *window, int key, int scancode, int action, int mods) {
    using namespace openGL::scene;

    bool set = action != GLFW_RELEASE && GLFW_REPEAT;
    switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwSetWindowShouldClose(window, GL_TRUE);
            break;
        case GLFW_KEY_W:
            prog->g_cameraUpdate.set(CameraUpdate::moveForward, set);
            break;
        case GLFW_KEY_S:
            prog->g_cameraUpdate.set(CameraUpdate::moveBackward, set);
            break;
        case GLFW_KEY_A:
            prog->g_cameraUpdate.set(CameraUpdate::moveLeft, set);
            break;
        case GLFW_KEY_D:
            prog->g_cameraUpdate.set(CameraUpdate::moveRight, set);
            break;
        case GLFW_KEY_Q:
            prog->g_cameraUpdate.set(CameraUpdate::moveDown, set);
            break;
        case GLFW_KEY_E:
            prog->g_cameraUpdate.set(CameraUpdate::moveUp, set);
            break;
        case GLFW_KEY_UP:
            prog->g_cameraUpdate.set(CameraUpdate::rotateUp, set);
            break;
        case GLFW_KEY_DOWN:
            prog->g_cameraUpdate.set(CameraUpdate::rotateDown, set);
            break;
        case GLFW_KEY_LEFT:
            if (mods == GLFW_MOD_SHIFT)
                prog->g_cameraUpdate.set(CameraUpdate::rollLeft, set);
            else
                prog->g_cameraUpdate.set(CameraUpdate::rotateLeft, set);
            break;
        case GLFW_KEY_RIGHT: // there is an error with roll right but only with the left shift ******************
            if (mods == GLFW_MOD_SHIFT)
                prog->g_cameraUpdate.set(CameraUpdate::rollRight, set);
            else
                prog->g_cameraUpdate.set(CameraUpdate::rotateRight, set);
            break;
        case GLFW_KEY_SPACE:
            prog->g_play = set ? !prog->g_play : prog->g_play;

#if SOUND_ENABLE
            if (!prog->g_play) {
                // turn of the music playing
                prog->mediaPlayer->setAllSoundsPaused(true);
                prog->liftAudioPlaying = false;
                prog->roarAudioPlaying = false;
            } // sound will resume on it's own in oncePerFrame()
#endif

            break;
        case GLFW_KEY_R:
            if (mods == GLFW_MOD_CONTROL)
                prog->g_play = true;
            break;
        case GLFW_KEY_F:
            if (mods == GLFW_MOD_CONTROL && set)
                prog->oncePerFrame();
            break;
        case GLFW_KEY_P:
            if (mods == GLFW_MOD_CONTROL)
                if (!prog->renderer->reloadShadersFromFile(prog->g_program))
                    cerr << "ERROR: shaders could were not read correctly\n";
            break;
        case GLFW_KEY_LEFT_BRACKET:
            if (mods == GLFW_MOD_SHIFT) {
                prog->g_rotationSpeed *= 0.5;
            } else {
                prog->g_panningSpeed *= 0.5;
            }
            break;
        case GLFW_KEY_RIGHT_BRACKET:
            if (mods == GLFW_MOD_SHIFT) {
                prog->g_rotationSpeed *= 1.5;
            } else {
                prog->g_panningSpeed *= 1.5;
            }
            break;

        // CAMERA VIEW UPDATES
        case GLFW_KEY_1:
            prog->CAMERA_ANGLE = CAR;
            break;
        case GLFW_KEY_2:
            prog->CAMERA_ANGLE = DISTANT;
            prog->g_camera = prog->CAM_DEFAULT; // reset to default
            prog->reloadViewMatrix();
            break;
        case GLFW_KEY_3:
            // camera tracks the car movement from strategic locations
            prog->CAMERA_ANGLE = TRACKING;
        default:
            break;
  }
}

/**
 * To reset the camera matrix back to the default value.
 *
 * This was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587.
 */
void GraphicsProgram::resetCamera(openGL::scene::Camera &camera) {
    camera = openGL::scene::Camera(math::Vec3f{0.f, 0.f, 5.f},
                                   math::Vec3f{0.f, 0.f, -1.f},
                                   math::Vec3f{0.f, 1.f, 0.f});
}


//==================== OPENGL HELPER FUNCTIONS ====================//

/**
 * To return the error code if OpenGL has an error in operation
 *
 * This was borrowed from Andrew Owens from the boilerplate code
 * provided in CPSC 587
 */
string GL_ERROR() {
    GLenum code = glGetError();

    switch (code) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            return "Non Valid Error Code";
    }
}
