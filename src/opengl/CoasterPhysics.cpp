/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Some of the code used here was borrowed from the lecture notes provided by Dr. P
 * and Andrew Owens tutorial notes from CPSC 587.
 */

#include <iostream>
#include <vector>
#include <cmath>

#include "mat4f.h"
#include "CoasterPhysics.h"
#include "Geometry.h"
#include "curve.h"

#define DEBUG 0

namespace math {
namespace physics {

using namespace std;

/**************************************** ARC LENGTH PARAM FUNCTIONS ***********************************************/

/**
 * To reparameterize the curve based on the subdivision value of N passed in.
 */
math::geometry::Curve ttlArcLengthReParam(const math::geometry::Curve &curve, int N) {
    vector<math::Vec3f> uValue; // store the distance for each point of deltaS
    math::Vec3f Pcurr; // store current point each iteration
    double deltaS = (double)length(curve) / (double)N; // length of delta S between points
    double curLength = 0.0f; // starting point for getting distances

    uValue.push_back(curve[0]); // store the first point

    // iterate through and calculate all the u values for the new param. curve
    for (unsigned int i = 1; i < curve.pointCount(); i++) {
        Pcurr = curve[i];
        math::Vec3f prev = curve[i-1];
        curLength += distance(Pcurr, prev);

        if (curLength >= deltaS) { // check to see if we have surpassed delta s
            uValue.push_back(Pcurr);
            curLength = 0.0f; // reset for next point
        }
    }

    return math::geometry::Curve(uValue, true);
}




/******************************************** TRACK FUNCTIONS ****************************************/

/**
 * Find the heighest point on the roller coaster and return its value (y).
 */
double getMaxHeight(const math::geometry::Curve &curve) {
    return curve[getMaxIndex(curve)].m_y;
}

/**
 * Get the highest point on the roller coaster and return its index.
 */
int getMaxIndex(const math::geometry::Curve &curve) {
    double max = curve.pointCount() > 0 ? curve[0].m_y : -1.0f;
    if (max < 0.0)
        return -1; // error in curve geometry
    unsigned int index = 0;

    for (unsigned int i = 0; i < curve.pointCount(); i++) {
        if (curve[i].m_y > max) {
            max = curve[i].m_y; // record the new talest height
            index = i; // record the tallest index
        }
    }
    return index;
}

/**
 * Get the lowest point of the roller coaster and return its value in height (y).
 */
double getMinHeight(const math::geometry::Curve &curve) {
    return curve[getMinIndex(curve)].m_y;
}

/**
 * Get the lowest point on the roller coaster and return its index.
 */
int getMinIndex(const math::geometry::Curve &curve) {
    double min = curve.pointCount() > 0 ? curve[0].m_y : -1.0f;
    if (min < 0.0)
        return -1; // error in curve geometry
    unsigned int index = 0;

    for (unsigned int i = 0; i < curve.pointCount(); i++) {
        if (curve[i].m_y < min) {
            min = curve[i].m_y; // record the new talest height
            index = i; // record the tallest index
        }
    }
    return index;
}

/**
 * Calculate the velocity of the train based on the conservation
 * of energy equation.
 */
double getFreefallVelocity(double maxHeight, double curHeight) {
    return sqrt(2 * GRAVITY * (maxHeight - curHeight));
}

/**
 * To get the deceleration value based on the current position compared
 * to the end of the deceleration stage
 */
double getDecelerationVelocity(const math::Vec3f &startPos,
                               const math::Vec3f &curPos,
                               const math::Vec3f &endPos,
                               double velocity) {
    double L = abs(distance(startPos, endPos));
    double Ddec = abs(distance(curPos, endPos));
    return velocity * ((double)Ddec / (double)L);
}

/**
 * Get the speed of the cart at the given index
 */
double v(const math::geometry::Curve &curve, unsigned int index) {
    static double curVelocity = 0.0f;
    PHASE phase;
    uint cartOffset = 500; // add some length to line it up nicely
    math::Vec3f pos = curve[index];
    double speed;

    // determine the current phase
    if (index >= LIFT_START || index <= getMaxIndex(curve) + 150) // offset for making sure the roller coaster starts the fall sufficiently quickly
        phase = LIFT;
    else if (index >= DECEL_START && index <= LIFT_START)
        phase = END;
    else
        phase = FALL;


    // get the speed based on the phase
    switch(phase) {
    case LIFT:
        speed = LIFT_SPEED; // constant lift velocity
        break;
    case FALL:
        speed = getFreefallVelocity(getMaxHeight(curve), pos.m_y) + LIFT_SPEED / 2; // add a little extra to simiulate the speed coming of the chain
        curVelocity = speed;
        break;
    case END:
        // constant deceleration velocity
        speed = getDecelerationVelocity(curve[DECEL_START], pos, curve[LIFT_START + cartOffset], curVelocity);
        break;
    }

    return speed;
}


/**
 * Returns the index of the next position based on the speed being traveled.
 */
unsigned int getPosition(const math::geometry::Curve &curve,
                         unsigned int cur,
                         double dv,
                         double dt) {
    double deltaS = (double)length(curve) / (double)curve.pointCount(); // distance between each point
    double ds = getDistance(dv, dt); // get distance to travel
    unsigned int index = cur + (unsigned int)(ds / deltaS); // get the index value and truncate to the index
#if DEBUG
    cout << "INDEX: " << index << endl;
#endif
    return index % curve.pointCount(); // wrap around if needed
}

/**
 * To take the derivative of position based on the time stamp
 * to get the current positions velocity.
 */
math::Vec3f getVelocity(const math::geometry::Curve &curve,
                        unsigned int cur,
                        double dt) {
    unsigned int next = getPosition(curve, cur, v(curve, cur), dt);
    return ((curve[next] - curve[cur]) / dt); // finite difference between current position and next
}

/**
 * To take the derivative of velocity to return the acceleration
 * of the current position.
 */
math::Vec3f getAcceleration(const math::geometry::Curve &curve,
                            unsigned int cur,
                            double dt) {
    unsigned int nextPos = getPosition(curve, cur, v(curve, cur), dt);
    math::Vec3f next = getVelocity(curve, nextPos, dt);
    math::Vec3f current = getVelocity(curve, cur, dt);

    return (next - current) / dt; // finite difference between current velocity and next velocity

}

/**
 * get the distance traveled based on the time stamp and projected
 * velocity
 */
double getDistance(double velocity, double deltaTime) {
    return velocity * deltaTime;
}







/****************************** TRACK GENERATION **************************************/

/**
 * To trace the track and figure out based on the each fixed point, what the
 * orientation of the track is.
 */
void generateTrack(const math::geometry::Curve &curve,
                                  opengl::Geometry &track,
                                  double deltaTime) {
    const unsigned int granularity = 200; // how coarse the track is displayed
    const double trackWidth = 0.05;

    math::Vec3f start1, start2;
    math::Vec3f normal, tangent, binormal;
    math::Vec3f cur, point;

    // go through every so many points to create a rought approximation of the track
    for (unsigned int i = 0; i < curve.pointCount()-1; i += granularity) {
        // get the normal, tangent and binormal
        cur = curve[i]; // retrieve the point on the track
        normal = getNormal(curve, i, deltaTime);
        tangent = getTangent(curve, i, deltaTime);
        binormal = cross(tangent, normal);
        normal = cross(binormal, tangent); // make sure normal is indeed orthogonal

        // create right side of track
        point = cur + (normalized(binormal) * trackWidth); // add the points to get width
        track.verts.push_back(point);
        track.normals.push_back(normal);
        start1 = i == 0 ? point : start1; // store the first point for later
        // create left side of track
        point = cur - (normalized(binormal) * trackWidth);
        track.verts.push_back(point);
        track.normals.push_back(normal);
        start2 = i == 0 ? point : start2; // store the first point for later

    }
    // complete the loop
    track.verts.push_back(start1);
    track.normals.push_back(normal);
    track.verts.push_back(start2);
    track.normals.push_back(normal);
}

/**
 * To generate the support beams needed for the track and angle them out if the track
 * normal is pointed in the negative y direction.
 */
void generateSupports(const math::geometry::Curve &curve,
                      opengl::Geometry &supports,
                      double deltaTime) {

    const unsigned int granularity = 1000; // how coarse the track is displayed
    math::Vec3f support;
    math::Vec3f normal, tangent, binormal, newSupport;

    // go through every so many points to create a rought approximation of the track supports
    for (unsigned int i = 0; i < curve.pointCount(); i += granularity) {
        normal = getNormal(curve, i, deltaTime);
        tangent = getTangent(curve, i, deltaTime);
        binormal = cross(tangent, normal);
        normal = cross(binormal, tangent);

        bool pos1 = (i >= 22000 && i <= 25500);

        if (normal.m_y < 0.0) {
            if ( pos1 ) { // if not in the loop
                continue; // go to the next point and skip this

            } else { // create angled support for areas where the track noraml is facing down
                support = curve[i];
                supports.verts.push_back(support);
                supports.normals.push_back(math::Vec3f(0,0,0));

                newSupport = support + (normalized(-normal) * 0.2);
                for (unsigned int i = 0; i < 2; i++) { // push back twice for proper gl_lines
                    supports.verts.push_back(newSupport);
                    supports.normals.push_back(math::Vec3f(0,0,0));
                }

                support.m_y = -0.1; // get the ground position directly below
                supports.verts.push_back(math::Vec3f(newSupport.m_x, -0.1, newSupport.m_z));
                supports.normals.push_back(math::Vec3f(0,0,0));
            }

        } else {
            if ( !pos1 ) { // if not in the loop
                support = curve[i];
                supports.verts.push_back(support);
                supports.normals.push_back(math::Vec3f(0,0,0));
                support.m_y = -0.1; // get the ground position directly below
                supports.verts.push_back(support);
                supports.normals.push_back(math::Vec3f(0,0,0));
            }
        }
    }
}





/********************************** ORIENTATION **********************************************/

/**
 * To take a cart position and figure out on the track what the cart orientation should be
 */
math::Mat4f getOrientation(const math::geometry::Curve &curve, unsigned int pos, double deltaTime) {
    math::Vec3f normal = getNormal(curve, pos, deltaTime);
    math::Vec3f tangent = getTangent(curve, pos, deltaTime);
    math::Vec3f binormal = normalized(cross(tangent, normal));
    normal = cross(binormal, tangent); // make sure normal is indeed orthogonal

    Mat4f rotationMatrix = {binormal.m_x, normal.m_x, tangent.m_x, 0,
                            binormal.m_y, normal.m_y, tangent.m_y, 0,
                            binormal.m_z, normal.m_z, tangent.m_z, 0,
                            0.0f, 0.0f, 0.0f, 1.0f};

    return rotationMatrix;
}

/**
 * Get the surface normal to the current point
 */
math::Vec3f getNormal(const math::geometry::Curve &curve, unsigned int pos, double deltaTime) {
    math::Vec3f cAccel = getAcceleration(curve, pos, deltaTime); // do not multiply by velocity squared
    math::Vec3f normal = normalized(cAccel + math::Vec3f(0.0f, GRAVITY, 0.0f));
    return normal;
}

/**
 * Get the surface tangent to the current point
 */
math::Vec3f getTangent(const math::geometry::Curve &curve, unsigned int pos, double deltaTime) {
    math::Vec3f cur = curve[pos];
    math::Vec3f next = curve[getPosition(curve, pos, v(curve, pos), deltaTime)]; // get the next position based on speed
    math::Vec3f tangent = normalized(next - cur);
    return tangent;
}

/**
 * Get the surface binormal to the current point
 */
math::Vec3f getBinormal(const math::geometry::Curve &curve, unsigned int pos, double deltaTime) {
    math::Vec3f normal = getNormal(curve, pos, deltaTime);
    math::Vec3f tangent = getTangent(curve, pos, deltaTime);
    math::Vec3f binormal = normalized(math::cross(tangent, normal));

    return binormal;
}
} // namespace physics
} // namespace math
