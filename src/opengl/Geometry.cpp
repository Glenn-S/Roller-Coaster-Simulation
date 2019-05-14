/**
 * Authoer: Glenn Skelton
 * Modified: Feb 4, 2019
 *
 * This code was borrowed in inspiration from the boilerplate codde provided
 * by Andrew Owens for CPSC 587 and from John Hall's boilerplate code from
 * CPSC 453.
 */

#include "Geometry.h"

namespace opengl {

Geometry::Geometry() :
    vaoID(0),
    vertexBufferID(0),
    normalBufferID(0),
    colourBufferID(0),
    uvBufferID(0),
    indexBufferID(0),
    verticesCount(0),
    indicesCount(0),
    modelMatrix(math::identity()) {}

Geometry::~Geometry() {
    verts.clear();
}
} // namespace opengl
