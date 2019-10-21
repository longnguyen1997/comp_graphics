:: This is the script we will test your submission with.

set SIZE=800 800
set BIN=sample_solution\win\a4.exe

%BIN% -size %SIZE% -input data/scene01_plane.txt  -output out\a01.png -normals out\a01n.png -depth 8 18 out\a01d.png
%BIN% -size %SIZE% -input data/scene02_cube.txt   -output out\a02.png -normals out\a02n.png -depth 8 18 out\a02d.png
%BIN% -size %SIZE% -input data/scene03_sphere.txt -output out\a03.png -normals out\a03n.png -depth 8 18 out\a03d.png
%BIN% -size %SIZE% -input data/scene04_axes.txt   -output out\a04.png -normals out\a04n.png -depth 8 18 out\a04d.png
%BIN% -size %SIZE% -input data/scene05_bunny_200.txt -output out\a05.png -normals out\a05n.png -depth 0.8 1.0 out\a05d.png
%BIN% -size %SIZE% -input data/scene06_bunny_1k.txt -bounces 4 -output out\a06.png -normals out\a06n.png -depth 8 18 out\a06d.png
%BIN% -size %SIZE% -input data/scene07_arch.txt -bounces 4 -shadows -output out\a07.png -normals out\a07n.png -depth 8 18 out\a07d.png
