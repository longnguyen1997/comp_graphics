#ifndef PARSE_H
#define PARSE_H

#include "curve.h"
#include "surf.h"
#include <iostream>
#include <string>

/* This function implements a parser for the "SWP" file format.  It's
   something Eugene came up with specifically for this assigment.

   A SWP file allows you to describe spline curves and swept surfaces.
   To specify a curve, you use the following syntax:

   TYPE NAME STEPS NUMPOINTS
   [ CONTROLPOINT ]
   [ CONTROLPOINT ]
   ...

   ---CURVES---
   
   TYPE can be Bez2, Bez3, Bsp2, Bsp3 which specify Bezier/Bspline
   curves in 2/3 dimensions.

   NAME is just a term that can be used later to refer to the curve.
   You can create an anonymous curve by giving '.' (period) as the
   name.

   STEPS controls how finely the curve is discretized.  Specifically,
   each cubic piece (not the whole curve) will be discretized into
   STEPS segments.

   NUMPOINTS indicates the number of control points.

   Each CONTROLPOINT is given as [ x y ] for 2D curves, and [ x y z ]
   for 3D curves.  Note that the square braces are required.

   In addition to these curves, you can specify circles as follows:

   circ NAME STEPS RADIUS

   The variables are self-explanatory.

   ---SURFACES---

   Surfaces of revolution are defined as follows:

   srev NAME STEPS PROFILE

   PROFILE is the name of a curve that previously occurred in the
   file.  This name *must* refer to a 2D curve (an error will be
   returned if a 3D curve is provided).

   Finally, generalized cylinders are defined as follows:

   gcyl NAME PROFILE SWEEP

   As with surfaces of revolution, PROFILE is the name of a 2D curve.
   SWEEP is the name of a 2D *or* 3D curve.
*/

// The vectors are passed in by reference.  parseFile actually writes
// to these variables.  This is how we pull off a multiple
// return-value function.
bool parseFile(std::istream &in,
               std::vector<std::vector<Vector3f> > &ctrlPoints, 
               std::vector<Curve>                  &curves,     
               std::vector<std::string>            &curveNames, 
               std::vector<Surface>                &surfaces,   
               std::vector<std::string>            &surfaceNames );

#endif
