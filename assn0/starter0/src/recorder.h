#ifndef RECORDER_H
#define RECORDER_H

#include <vector>
#include <vecmath.h>

class GeometryRecorder{
public:
    GeometryRecorder(int nverts);
    // write a vertex into the CPU buffer
    void record(Vector3f pos,
                Vector3f normal);
    // draw recorded points
    void draw();
    // empties the recording buffer.
    void clear();

private:
    int m_nverts;
    int m_current;
    std::vector<Vector3f> m_position;
    std::vector<Vector3f> m_normal;
};

#endif
