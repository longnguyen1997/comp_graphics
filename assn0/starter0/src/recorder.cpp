#include "recorder.h"

#include <cassert>
#include <cstdint>
#include "gl.h"

GeometryRecorder::GeometryRecorder(int nverts)
    : m_nverts(nverts), m_current(0)
{
    m_position.resize(nverts);
    m_normal.resize(nverts);
}

void GeometryRecorder::record(Vector3f pos,
            Vector3f normal)
{
    assert(m_current < m_nverts);
    m_position[m_current] = pos;
    m_normal[m_current] = normal;
    m_current++;
}

void GeometryRecorder::draw()
{
    if (m_current % 3) 
    {
        printf("Warning: number of vertices not divisible by 3\n");
    } else if (m_current == 0) 
    {
        printf("Warning: no vertices recorded\n");
        return;
    }
    
    // upload data to GPU
    uint32_t vertexarray;
    glGenVertexArrays(1, &vertexarray);
    glBindVertexArray(vertexarray);
    uint32_t vertexbuffer[2];
    glGenBuffers(2, vertexbuffer);

    // POSITION
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[0]);
    size_t position_nbytes = m_nverts * sizeof(m_position[0]);
    glBufferData(GL_ARRAY_BUFFER, position_nbytes, 
        m_position.data(), GL_DYNAMIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0,
 	3,
 	GL_FLOAT,
 	GL_FALSE,
 	sizeof(m_position[0]),
 	(void*)0);

    // NORMALS
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[1]);
    size_t normal_nbytes = m_nverts * sizeof(m_normal[0]);
    glBufferData(GL_ARRAY_BUFFER, normal_nbytes, 
        m_normal.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1,
 	3,
 	GL_FLOAT,
 	GL_FALSE,
 	sizeof(m_normal[0]),
 	(void*)0);

    // Everything is uploaded.
    // Now draw.
    glDrawArrays(GL_TRIANGLES, 0, m_current);

    // Release allocated buffers/arrays.
    glDeleteBuffers(2, vertexbuffer);
    glDeleteVertexArrays(1, &vertexarray);
}
void GeometryRecorder::clear()
{
    m_current = 0;
}
