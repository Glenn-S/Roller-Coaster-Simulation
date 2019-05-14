/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Parser for extracting vertices and normals from a .obj file format (in modified form by myself)
 *
 * This parser is a modification from one that I wrote for CPSC 453 asignment 5
 */

#ifndef MODEL_H
#define MODEL_H

#include <vector>
#include <iostream>

#include "Model.h"
#include "vec3f.h"
#include "Geometry.h"

using namespace std;

namespace scene {
namespace Model {

void modelParser(opengl::Geometry &object, string filename);

} // namespace Model
} // namespace scene


#endif // MODEL_H
