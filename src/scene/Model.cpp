/**
 * Author: Glenn Skelton
 * Modified: Feb 12, 2019
 *
 * Parser for extracting vertices and normals from a .obj file format (in modified form by myself)
 *
 * This parser is a modification from one that I wrote for CPSC 453 asignment 5
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <string.h>
#include <exception>

#include "Model.h"
#include "vec3f.h"
#include "Geometry.h"

using namespace std;
using namespace opengl;

namespace scene {
namespace Model {


/**
 * To read in an obj file and parse the vertices and normals of that model
 */
void modelParser(opengl::Geometry &object, string filename) {
    vector<math::Vec3f> verts;
    vector<math::Vec3f> normals;
    math::Vec3f vert;
    math::Vec3f normal;

    constexpr int buffSize = 80;
    char buffer[buffSize]; // standard line length

    int t1, t2;

    try { // open file and read contents
        ifstream infile;
        infile.open(filename);

        if (!infile) {
            cout << "Error openning " << filename << endl;
            exit(EXIT_FAILURE);
        }

        while (!infile.eof()) { // read in all contents
            infile.getline(buffer, buffSize);

            if (buffer[0] == 'v') { // get the line
                if (strstr(buffer, "vn") != NULL) {
                    sscanf(buffer, "vn %f %f %f", &normal.m_x, &normal.m_y, &normal.m_z);
                    normals.push_back(normal);
                } else {
                    sscanf(buffer, "v %f %f %f", &vert.m_x, &vert.m_y, &vert.m_z);
                    verts.push_back(vert);
                }
            } else if (buffer[0] == 'f') {
                sscanf(buffer, "f %d//%d", &t1, &t2);

                object.verts.push_back(verts[t1-1]); // store the vert
                object.normals.push_back(normals[t2-1]); // store the corresponding normal
            } else continue; // ignore everything else
        }
        infile.close();
    } catch (exception &e) {
        cout << e.what() << endl;
    }
}

} // namespace Model
} // namespace scene
