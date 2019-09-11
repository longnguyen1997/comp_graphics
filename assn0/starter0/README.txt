6.837 - PS0

1) How do I compile?

cd <top-level dir of start0>; mkdir build; cd build; cmake -j ..; make -j

Using the -j flag gives maximum performance; you can compile even through SSH
really, really quickly. This compilation works on SSH and local Athena PCs.

2) Collaborated?

No.

3) Useful references/websites?

No.

4) Known problems with the code?

I stopped doing the assignment after finishing 2.2. 2.3 and onwards
are incomplete, but this problem set doesn't count for points, so
I'll just do them when I have the time.

5) Extra credit completed?

None.

6) General comments?

Note that SSH with X11 forwarding does not work. This is because
SSH sessions to athena.dialup.mit.edu use VMWare's solution, which
provides a vGPU that isn't supported natively by Athena Ubuntu's
default system packages. In particular, libgl will complain.

-----------

APPENDIX

Cmake
 - CMakeLists.txt: contains the build information for the project.
 - build: the directory where your project will be built.

Source and Data
 - src:   starter code. Add your source code here.
 - data:  test files for your solution.
 - solution: example solution (binary).

Dependencies
 - vecmath: source code for vecmath library.
 - glew:  source code for GLEW library.
 - glfw:  source code for GLFW library.
