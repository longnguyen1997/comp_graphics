# This is the script we will test your submission with.

SIZE="8192 8192"
#BIN=./sample_solution/athena/a4
BIN=./build/a4
mkdir -p out_8k

${BIN} -size ${SIZE} -input data/scene01_plane.txt  -output out_8k/a01.png -normals out_8k/a01n.png -depth 8 18 out_8k/a01d.png
${BIN} -size ${SIZE} -input data/scene02_cube.txt   -output out_8k/a02.png -normals out_8k/a02n.png -depth 8 18 out_8k/a02d.png
${BIN} -size ${SIZE} -input data/scene03_sphere.txt -output out_8k/a03.png -normals out_8k/a03n.png -depth 8 18 out_8k/a03d.png
${BIN} -size ${SIZE} -input data/scene04_axes.txt   -output out_8k/a04.png -normals out_8k/a04n.png -depth 8 18 out_8k/a04d.png
${BIN} -size ${SIZE} -input data/scene05_bunny_200.txt -output out_8k/a05.png -normals out_8k/a05n.png -depth 0.8 1.0 out_8k/a05d.png
${BIN} -size ${SIZE} -input data/scene06_bunny_1k.txt -bounces 4 -output out_8k/a06.png -normals out_8k/a06n.png -depth 8 18 out_8k/a06d.png
${BIN} -size ${SIZE} -input data/scene07_arch.txt -bounces 4 -shadows -output out_8k/a07.png -normals out_8k/a07n.png -depth 8 18 out_8k/a07d.png
