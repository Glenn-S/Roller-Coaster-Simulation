/**
 * Author: Glenn Skelton
 * Modified: February 4, 2019
 *
 * This code was modified and borrowed from both the boilerplate provided
 * by Andrwe Owens  in CPSC 587 and the boilerplate code provided by
 * John Hall for CPSC 453.
 */

#include <vector>
#include <iostream>

#include "program.h"
#include "RenderingEngine.h"
#include "Geometry.h"

namespace opengl {

RenderingEngine::RenderingEngine() {}
RenderingEngine::~RenderingEngine() {}

/**
 * Generate the GPU ID's at the beginning of the program
 *
 * Borrowed from John Halls CPSC 453 boilerplate code and
 * Andrew Owens boilerplate code from CPSC 587.
 */
void RenderingEngine::assignBuffer(Geometry &geometry) {
    glGenVertexArrays(1, &geometry.vaoID);
    glGenBuffers(1, &geometry.vertexBufferID);
    glGenBuffers(1, &geometry.normalBufferID);
    glGenBuffers(1, &geometry.colourBufferID);

    glGenBuffers(1, &geometry.indexBufferID);
}

/**
 * Destroy the GPU ID's at the end of the program
 *
 * Modified from Andrew Owens CPSC 587 boilerplate code
 */
void RenderingEngine::deleteBuffer(Geometry &geometry) {
    glDeleteVertexArrays(1, &geometry.vaoID);
    glDeleteBuffers(1, &geometry.vertexBufferID);
    glDeleteBuffers(1, &geometry.vertexBufferID);
    glDeleteBuffers(1, &geometry.normalBufferID);
    glDeleteBuffers(1, &geometry.colourBufferID);

    glDeleteBuffers(1, &geometry.indexBufferID);
}

/**
 * Create the VAO's and VBO's
 */
void RenderingEngine::setBufferData(Geometry &geometry) {
    // bind vertices
    glBindVertexArray(geometry.vaoID);
    glBindBuffer(GL_ARRAY_BUFFER, geometry.vertexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, geometry.indexBufferID);
    glVertexAttribPointer(0,        // attribute layout # above
                          3,        // # of components (ie XYZ )
                          GL_FLOAT, // type of components
                          GL_FALSE, // need to be normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );
    glEnableVertexAttribArray(0); // match layout # in shader

    // bind normals
    glBindBuffer(GL_ARRAY_BUFFER, geometry.normalBufferID);
    glVertexAttribPointer(1,        // attribute layout # above
                          3,        // # of components (ie XYZ )
                          GL_FLOAT, // type of components
                          GL_FALSE, // need to be normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );
    glEnableVertexAttribArray(1);

    // bind colours
    glBindBuffer(GL_ARRAY_BUFFER, geometry.colourBufferID);
    glVertexAttribPointer(2,        // attribute layout # above
                          3,        // # of components (ie XYZ )
                          GL_FLOAT, // type of components
                          GL_FALSE, // need to be normalized?
                          0,        // stride
                          (void *)0 // array buffer offset
    );
    glEnableVertexAttribArray(2);

    glBindVertexArray(0); // reset to default
}

/**
 * This goes through the trouble of loading and reloading the shader files
 *
 * Borrowed from Andrew Owens from CPSC 587 boilerplate code.
 */
bool RenderingEngine::reloadShadersFromFile(std::vector<opengl::Program> &g_program) {
    // will delete shaders from GPU as well (RAII)
    g_program.clear();

    using namespace opengl;
    // shader ID from OpenGL
    //auto vsSource = loadShaderStringFromFile("./shaders/basic_vs.glsl");
    //auto fsSource = loadShaderStringFromFile("./shaders/basic_fs.glsl");

    auto vsSource = loadShaderStringFromFile("phong_vs.glsl");
    auto fsSource = loadShaderStringFromFile("phong_fs.glsl");

    if (vsSource.empty() || fsSource.empty()) {
        std::cerr << "Failed to load shaders from file\n";
        return false;
    }
    auto program = makeProgram(vsSource, fsSource);
    if (!program.isValid()) {
        std::cerr << "Failed to load program\n";
        return false;
    }
    g_program.push_back(std::move(program));

    return true;
}
} // namespace opengl
