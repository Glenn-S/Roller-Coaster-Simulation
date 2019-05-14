/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Some of the code used here was borrowed from the lecture notes provided by Dr. P
 * and Andrew Owens tutorial notes from CPSC 587.
 */

#ifndef COASTERPHYSICS_H
#define COASTERPHYSICS_H

#include "curve.h"
#include "Geometry.h"


using namespace std;

namespace math {
namespace physics {

enum PHASE {LIFT = 0, FALL = 1, END = 2};

/************************ CONSTANTS ****************************/

// TRACK DEFENITIONS
const unsigned int LIFT_START = 168000;
const unsigned DECEL_START = 140000;

// lift speed of incline
const double LIFT_SPEED = 1.0f;

const double GRAVITY = 9.81f; // m/s^2


/************************** FUNCTIONS ***************************/

//double getVelocity(double deltaTime, double deltaDistance);
double getVelocity(math::Vec3f pos);
double getFreefallVelocity(double maxHeight, double curHeight);
double getDecelerationVelocity(const math::Vec3f &startPos,
                               const math::Vec3f &curPos,
                               const math::Vec3f &endPos,
                               double velocity);
double getDistance(double velocity, double deltaTime);

// FRAMING THE CURVE
double v(const math::geometry::Curve &curve, unsigned int index);
unsigned int getPosition(const math::geometry::Curve &curve,
                         unsigned int cur,
                         double dv,
                         double dt);
math::Vec3f getVelocity(const math::geometry::Curve &curve,
                        unsigned int cur,
                        double dt);
math::Vec3f getAcceleration(const math::geometry::Curve &curve,
                            unsigned int cur,
                            double dt);



// ARC LENGTH REPARAM
math::geometry::Curve ttlArcLengthReParam(const math::geometry::Curve &curve, int numDivisions);


// TRACK ORIENTATIONS
void generateTrack(const math::geometry::Curve &curve,
                                  opengl::Geometry &track,
                                  double deltaTime);
void generateSupports(const math::geometry::Curve &curve,
                                     opengl::Geometry &supports,
                                     double deltaTime);
math::Mat4f getOrientation(const math::geometry::Curve &curve, unsigned int pos, double deltaTime);


// ORIENTATION
math::Vec3f getNormal(const math::geometry::Curve &curve, unsigned int pos, double deltaTime);
math::Vec3f getTangent(const math::geometry::Curve &curve, unsigned int pos, double deltaTime);
math::Vec3f getBinormal(const math::geometry::Curve &curve, unsigned int pos, double deltaTime);


double getMaxHeight(const math::geometry::Curve &curve);
int getMaxIndex(const math::geometry::Curve &curve);
double getMinHeight(const math::geometry::Curve &curve);
int getMinIndex(const math::geometry::Curve &curve);

} // namespace physics
} // namespace math



#endif // COASTERPHYSICS_H
