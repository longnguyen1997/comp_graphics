#include "vertexrecorder.h"

#include <cassert>
#include <cstdint>
#include "gl.h"

#ifndef M_PIf
#define M_PIf 3.141592f
#endif

VertexRecorder::VertexRecorder() :m_nverts(0)
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

void drawSphere(float r, int slices, int stacks) {
    assert(slices > 1);
    assert(stacks > 1);
    assert(r > 0);

    // TODO reuse recorder if sphere meshing becomes a bottleneck.
    VertexRecorder rec;

    float phistep = M_PIf * 2 / slices;
    float thetastep = M_PIf / stacks;

    for (int vi = 0; vi < stacks; ++vi) { // vertical loop
        float theta = vi * thetastep;
        float theta_next = (vi + 1) * thetastep;

        float z = r * cosf(theta);
        float z_next = r*cosf(theta_next);
        for (int hi = 0; hi < slices; ++hi) { // horizontal loop
            float phi = hi * phistep;
            float phi_next = (hi + 1) * phistep;

            float x = r * cosf(phi) * sinf(theta);
            float y = r * sinf(phi) * sinf(theta);

            Vector3f p1(r * cosf(phi) * sinf(theta), r * sinf(phi) * sinf(theta), z);
            Vector3f p2(r * cosf(phi_next) * sinf(theta), r * sinf(phi_next) * sinf(theta), z);
            Vector3f p3(r * cosf(phi_next) * sinf(theta_next), r * sinf(phi_next) * sinf(theta_next), z_next);
            Vector3f p4(r * cosf(phi) * sinf(theta_next), r * sinf(phi) * sinf(theta_next), z_next);

            Vector3f n1 = p1.normalized();
            Vector3f n2 = p2.normalized();
            Vector3f n3 = p3.normalized();
            Vector3f n4 = p4.normalized();

            rec.record(p1, n1); rec.record(p2, n2); rec.record(p3, n3);
            rec.record(p1, n1); rec.record(p3, n3); rec.record(p4, n4);
        }
    }
    rec.draw();
}
/*
void drawCube(float w) {
    assert(w >= 0);
    float wh = w / 2.0f;

    // TODO reuse recorder if cube meshing becomes a bottleneck.
    VertexRecorder rec;
    Vector3f nx1 = Vector3f(-wh, -wh, -wh);
    Vector3f nx2 = Vector3f(-wh, +wh, -wh);
    Vector3f nx3 = Vector3f(-wh, +wh, +wh);
    Vector3f nx4 = Vector3f(-wh, -wh, +wh);
    //rec.record(, );
    rec.record(Vector3f(-wh, wh, -wh), Vector3f(-1, 0, 0));
    rec.record(Vector3f(-wh, wh, wh), Vector3f(-1, 0, 0));


    Vector3f px1 = Vector3f(+wh, -wh, -wh);
    Vector3f px2 = Vector3f(+wh, +wh, -wh);
    Vector3f px3 = Vector3f(+wh, +wh, +wh);
    Vector3f px4 = Vector3f(+wh, -wh, +wh);

    rec.record(Vector3f(-wh, -wh, wh), Vector3f(-1, 0, 0));


    rec.draw();
}*/

void drawCylinder(int nsides, float r, float h) {
    assert(nsides >= 3);
    float step = 2 * M_PIf / nsides;

    VertexRecorder rec;
    std::vector<Vector3f> pos;
    std::vector<Vector3f> n;

    int posidx = 0;
    int nidx = 0;
    int uvidx = 0;
    int idxidx = 0;
    for (int face = 0; face < nsides; ++face) {
        float lx = r * cosf(face * step);
        float lz = r * sinf(face * step);
        pos.push_back(Vector3f(lx, 0.0f, lz));
        pos.push_back(Vector3f(lx, h, lz));

        n.push_back(Vector3f(cosf(face * step), 0.0f, sinf(face * step)));
        n.push_back(Vector3f(cosf(face * step), h, sinf(face * step)));

        //if (uv) {
            //uv[uvidx++] = (float)(face) / (nsides - 1);
            //uv[uvidx++] = 1.0f;
//
            //uv[uvidx++] = (float)(face) / (nsides - 1);
            //uv[uvidx++] = 0.0f;
        //}
    }
    for (int face = 0; face < nsides; ++face) {
        int i1 = face * 2;
        int i2;
        if (face == nsides - 1) {
            i2 = 1;
        } else {
            i2 = i1 + 3;
        }
        int i3 = i1 + 1;

        // draw
        rec.record(pos[i1], n[i1]);
        rec.record(pos[i2], n[i2]);
        rec.record(pos[i3], n[i3]);

        if (face == nsides - 1) {
            i2 = 0;
            i3 = 1;
        }
        else {
            i2 = i1 + 2;
            i3 = i1 + 3;
        }
        rec.record(pos[i1], n[i1]);
        rec.record(pos[i2], n[i2]);
        rec.record(pos[i3], n[i3]);
    }
    rec.draw();
}

void drawQuad(float w)
{
    VertexRecorder rec;
    float wh = w / 2;
    const Vector3f N(0, 1, 0);
    const Vector3f P1(-wh, 0, -wh);
    const Vector3f P2(+wh, 0, -wh);
    const Vector3f P3(+wh, 0, +wh);
    const Vector3f P4(-wh, 0, +wh);

    // first face
    rec.record(P1, N);
    rec.record(P2, N);
    rec.record(P3, N);

    // second face
    rec.record(P1, N);
    rec.record(P3, N);
    rec.record(P4, N);
    rec.draw();
}
