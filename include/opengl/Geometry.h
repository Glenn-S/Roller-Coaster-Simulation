/**
 * Authoer: Glenn Skelton
 * Modified: Feb 4, 2019
 *
 * This code was borrowed in inspiration from the boilerplate codde provided
 * by Andrew Owens for CPSC 587 and from John Hall's boilerplate code from
 * CPSC 453.
 */


#ifndef GEOMETRY_H
#define GEOMETRY_H

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include <iostream>
#include <vector>

#include "vec3f.h"
#include "mat4f.h"

using namespace std;

namespace opengl {

// Data needed rendering for mesh and line
class Geometry {
public:
    Geometry();
    virtual ~Geometry();

    vector<Geometry*> children; // scene graph

    // data structure for verts, normals, uv's, and colours
    vector<math::Vec3f> verts;
    vector<math::Vec3f> normals;
    vector<math::Vec3f> colours;
    vector<math::Vec3f> uvs;

    // Buffer ID's
    GLuint vaoID = 0;
    GLuint vertexBufferID = 0;
    GLuint normalBufferID = 0;
    GLuint uvBufferID = 0;
    GLuint colourBufferID = 0;
    GLuint indexBufferID = 0;

    GLuint verticesCount = 0;
    GLuint indicesCount = 0;

    math::Mat4f modelMatrix = math::identity();

    GLuint drawMode = 0; // draw mode for rendering ie. triangle mesh
    GLuint polygonMode = 0; // type of mesh, lines or fill eg.

    math::Vec3f colour = math::Vec3f(); // colour of the object

};
} // namespace opengl

#endif // GEOMETRY_H
