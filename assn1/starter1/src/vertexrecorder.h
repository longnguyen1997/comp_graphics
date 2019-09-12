#ifndef RECORDER_H
#define RECORDER_H

#include <vector>
#include <vecmath.h>
#include "gl.h"

class VertexRecorder{ 
public:
    VertexRecorder();
    // write a vertex into the CPU buffer
    void record(Vector3f pos,
                Vector3f normal);
    void record(Vector3f pos,
                Vector3f normal, 
		        Vector3f color);
    void record_poscolor(Vector3f pos,
		        Vector3f color);
    // draw recorded points
    void draw(GLenum mode = GL_TRIANGLES);
    // empties the recording buffer.
    void clear();
private:
    int m_nverts;
    std::vector<Vector3f> m_position;
    std::vector<Vector3f> m_normal;
    std::vector<Vector3f> m_color;
};

#endif
