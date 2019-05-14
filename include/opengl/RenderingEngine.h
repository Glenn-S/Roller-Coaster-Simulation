/**
 * Author: Glenn Skelton
 * Modified: February 4, 2019
 *
 * This code was modified and borrowed from both the boilerplate provided
 * by Andrwe Owens  in CPSC 587 and the boilerplate code provided by
 * John Hall for CPSC 453.
 */


#ifndef RENDERINGENGINE_H
#define RENDERINGENGINE_H

#include <vector>
#include <iostream>

#include "program.h"
#include "Geometry.h"

using namespace opengl;

namespace opengl {

class RenderingEngine {
public:
    RenderingEngine();
    ~RenderingEngine();

    void assignBuffer(Geometry &geometry);
    void deleteBuffer(Geometry &geometry);
    void setBufferData(Geometry &geometry);

    bool reloadShadersFromFile(std::vector<opengl::Program> &g_program);
};

} // namespace openGL

#endif // RENDERINGENGINE_H
