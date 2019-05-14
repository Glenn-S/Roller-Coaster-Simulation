CPSC 587 Assignment 1

Author: Glenn Skelton
Date: February 12, 2019


**NOTES**

Notes: Portions of this code have been inspired by or borrowed from
Andrew Owens using his boilerplate code and from John Hall's boilerplate
code from CPSC 453.

SOUND --------------------------------------------------------------------------
  Sound files for this program were obtained from
  lift.wav
    https://instrumentalfx.co/roller-coaster-sound-effect/
  roar.wav
    http://www.freesfx.co.uk
  The sound library used for this project was IrrKlang and the documentation
  used to run my engine was obtained from:
    https://www.ambiera.com/irrklang/docu/index.html

OBJ FILES ----------------------------------------------------------------------
  The obj files used in this project were designed by myself and modeled either
  in Blender or by hand by myself. They are located in the curves and models
  folder.

SHADER FILES -------------------------------------------------------------------
  The shader files used in this program were obtained from the boilerplate code
  provided by Andrew Owens for CPSC 587 Winter 2019.

PHYSICS ------------------------------------------------------------------------
  The physics model that I used for my program was based on the example code
  provided in class by Dr. Prusinkiewicz and through the notes provided in class
  and tutorial.

Notes on Sound:
Though there is sound, for whatever reason on the lab computers, it comes
out fuzzy where as on my personal computers there are little or no issue.


**INSTALL**

Load up in QT Creator and setup in release mode. Other modes will cause
the program to lag. This program was developed on a Linux environment
using the graphics lab computers.


**USER INTERFACE**

1   first person camera view (in car). All camera movement key calls are
        disabled.
2   distant view of track. All camera movement key calls are enabled.
3   third person view of roller coaster with camera that follows
	    the train. All camera movement key calls are enabled.


w	move forward
a	move left
s	move backward
d	move right
q	move down
e	move up

ctrl+left-click+drag	rotate camera around focus point

[	decrease move speed
]	increase move speed
{	decrease rotate speed
}	increase rotate speed

ctrl+r	run
space start/stop
esc	escape

To enable or disable audio, there is a macro in GraphicsProgram.cpp that can be
toggled depending. By default it is left on.


**PHYSICS/MATH**
To begin my curve is read in from a file containing points and is highly
subdivided (19 times) using cubic subdivision. following this, I pass the points
created into my arc length re-parameterization function which takes the total
length of the curve calculated and divides it by a value of N (ie. 1000) to
divide the length up. Stepping through each point, I accumulate the distance
based on my division size (deltaS) and accumulate the distance until deltaS has
been achieved. Once achieved I push the point back into a new curve and continue
until the whole curve has been re subdivided.

For getting the inclination of the tracks Frenet frame, I now pass the new
re-parameterized curve into generateTrack() which gets the inclination of the
track by calculating the acceleration at any finite moment along the track.
The tangent vector is obtained by looking forward on the track to the next point
based on the delta time (time per frame) and subtracts the current position from
it. The normal for the track is calculated by getting the acceleration at that
given point. This is calculated in getAcceleration() by getting the difference
between the velocity of the next point and subtracting the current positions
velocity (taking the derivative of velocity). Subsequently, velocity is obtained
in the same manner in getVelocity() by taking the difference between the next
projected position and the current position (the derivative of position). This
was based on our lecture notes provided. This same method is also used for
obtaining my carts orientation on the track through the function
getOrientation(). The getOrientation() function returns the model matrix for
updating the carts position. The binormal used is calculated from taking the
cross product of the normal and tangent and then the normal is recalculated by
crossing binormal with the tangent to ensure that the coordinates are
orthogonal. Using the basis calculated from the finite differences, I used
the resulting vectors, particularly, the binormal (normalized) is used to set a
distance for the width of the track. This method provides the correct orientation
because it used the normal vector and tangent to get the correct orientation.
Subsequently, the normal is correct because it is calculated by adding
the calculated acceleration to gravity to get the resultant vector between the
two forces. As a result this gives a vector that is the difference of the
forces being applied which happens to be normal to the track.

For determining the carts speed at any given point, there is a function called
v() which takes the carts position at any given moment and returns the speed
value based on the conservation of energy model which is calculated:
sqrt(2 * 9.81m/s^2 * [maxHeight - currentHeight])
This equation is used in getVelocity() to determine the index value of the next
position.

The deceleration of my roller coaster is determined by a function that takes the
value of the starting point of the deceleration section and the ending of the
deceleration section and, using the position of the cart, it gets the ratio of
how far the cart has traveled in this section and multiplies it by the last
recorded speed from just before entering the deceleration section.

I also used an epsilon value to slightly adjust the end of the lift section so
that the train moved in a more realistic manner to where the chain would end on
a real roller coaster. I also slightly overlapped the end of the deceleration
and beginning of the lift section so that the coaster slowed enough to look like
it was coming to a stop but to not sit there for a long time.

The resources that I sued to create my model were obtained from lecture and
tutorial notes taking in class along with example code that was posted on the
course website which was examined in class.
