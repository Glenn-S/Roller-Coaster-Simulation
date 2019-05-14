/**
 * Author: Andrew Owens
 *
 * This is a library for reading in a file containing a curve and storing it in
 * a Curve class. This code was borrowed from Andrew Owens from the CPSC 587
 * boilerplate code provided.
 */


#pragma once

#include <string>

#include "curve.h"

namespace math {
namespace geometry {

void saveCurveToFile(math::geometry::Curve const &curve, std::string const &filePath);

math::geometry::Curve loadCurveFromFile(std::string const &filePath);

math::geometry::Curve loadCurveFrom_OBJ_File(std::string const &filePath);

} // namespace geometry
} // namespace math
