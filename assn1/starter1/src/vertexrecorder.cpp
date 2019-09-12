#include "vertexrecorder.h"

#include <cassert>
#include <cstdint>
#include "gl.h"

VertexRecorder::VertexRecorder():m_nverts(0)
{
}

void VertexRecorder::record(Vector3f pos,
            Vector3f normal)
{
	record(pos, normal, Vector3f(1, 1, 1));
}
void VertexRecorder::record_poscolor(Vector3f pos,
	Vector3f color) {
	record(pos, Vector3f(0, 0, 0), color);
}
void VertexRecorder::record(Vector3f pos,
	Vector3f normal,
	Vector3f color) {
    m_position.push_back(pos);
    m_normal.push_back(normal);
    m_color.push_back(color);
	m_nverts++;
}

/* This implementation uploads data to the GPU on each draw call.
   A more efficient implementation would only upload when the vertex
   data changed. 
*/
void VertexRecorder::draw(GLenum mode)
{
	if (m_nverts == 0) {
		return;
	}
    // upload data to GPU
    uint32_t vertexarray;
    glGenVertexArrays(1, &vertexarray);
    glBindVertexArray(vertexarray);
    uint32_t vertexbuffer[3];
    glGenBuffers(3, vertexbuffer);

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

    // COLOR
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer[2]);
    size_t color_nbytes = m_nverts * sizeof(m_color[0]);
    glBufferData(GL_ARRAY_BUFFER, color_nbytes, 
        m_color.data(), GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2,
 	3,
 	GL_FLOAT,
 	GL_FALSE,
 	sizeof(m_color[0]),
 	(void*)0);

    // Everything is uploaded.
    // Now draw.
    glDrawArrays(mode, 0, m_nverts);

    // Release allocated buffers/arrays.
    glDeleteBuffers(3, vertexbuffer);
    glDeleteVertexArrays(1, &vertexarray);
}
void VertexRecorder::clear()
{
	m_nverts = 0;
	m_position.clear();
	m_normal.clear();
	m_color.clear();
}
