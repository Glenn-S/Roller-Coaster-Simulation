/**
 * Author: Glenn Skelton
 * Modified: February 4, 2019
 *
 * This is a program that reads in a curve file and produces a roller coaster
 * simulation that is modelled using physics.
 *
 * Inspiration for the structure of my code has been borrowed in part from the
 * boilerplate code provided by Andrew Owens for CPSC 587 2019 and John Hall from
 * the boilerplate code provided in CPSC 453 Fall 2018.
 *
 *
 * ==================================== CREDITS ====================================
 *
 * SOUND ---------------------------------------------------------------------------
 * Sound files for this program were obtained from
 * lift.wav
 *   https://instrumentalfx.co/roller-coaster-sound-effect/
 * roar.wav
 *   http://www.freesfx.co.uk
 *
 * The sound library used for this project was irrKlang and the documentation
 * used to run my engine was obtained from:
 *   https://www.ambiera.com/irrklang/docu/index.html
 *
 * OBJ FILES ------------------------------------------------------------------------
 * The obj files used in this project were designed by myself and modelled either
 * in Blender or by hand by myself. They are located in the curves and models folder.
 *
 * SHADER FILES ---------------------------------------------------------------------
 * The shader files used in this program were obtained from the boilerplate code
 * provided by Andrew Owens for CPSC 587 Winter 2019.
 *
 * PHYSICS --------------------------------------------------------------------------
 * The physics model that I used for my program was based on the example code
 * provided in class by Dr. Prusinkiewicz and through the notes provided in class and
 * tutorial.
 */

#include <iostream>

#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "GraphicsProgram.h"

using namespace std;

/**
 * Starts the graphics program to generate and run the roller coaster program.
 */
int main(int argc, char* argv[]) {
    GraphicsProgram *program = new GraphicsProgram("CPSC 587 Assignment 1");
    program->start();
    delete program;

    return EXIT_SUCCESS;
}
