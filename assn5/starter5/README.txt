Long Nguyen, lpn@mit.edu
6.837 - Computer Graphics, Fall 2019
Assignment 5 - Textures & Shadows

1. Build Instructions (Athena with Makefile)
==================
$ mkdir build
$ cd build
$ cmake -j ..
$ make -j

2. Collaboration

Absolutely none.

3. References

Just the lecture slides. Had to dig around
GLSL documentation a little bit too to
fully understand what was happening.

4. Known problems

For shadow visibility, I went with a Piazza
post. My previous submission arbitrarily used
60% as the visibility for in-shadow fragments.
I'm not sure if the Piazza post (by Darius) is
100% correct either, but it uses the depth of
x_tex as a scaling factor and doesn't use
the ambient color for the shadow. I hope this
counts somewhat.

5. Extra credit

None.

6. Comments

Please introduce students to GLSL before
making us dive right in! Similar to C++,
yes, but still tricky to read and grasp
at first.

