#include "particlesystem.h"

#include "gl.h"
#include "camera.h"
#include <random>
#include <cstdio>

float rand_uniform(float low, float hi) {
   float abs = hi - low;
   float f = (float)rand() / RAND_MAX;
   f *= abs;
   f += low;
   printf("rand %.2f\n", f);
   return f;
}

GLProgram::GLProgram(uint32_t apl, uint32_t apc, Camera* ac)
    : program_light(apl), program_color(apc), camera(ac) 
{
    enableLighting();
}
void GLProgram::updateModelMatrix(Matrix4f M) const
{
    camera->SetUniforms(active_program, M);
}
void GLProgram::enableLighting() {
    active_program = program_light;
    glUseProgram(active_program);
}
void GLProgram::disableLighting() {
    active_program = program_color;
    glUseProgram(active_program);
}
void GLProgram::updateMaterial(Vector3f diffuseColor,
    Vector3f ambientColor,
    Vector3f specularColor,
    float shininess,
    float alpha) const {
    int loc = glGetUniformLocation(active_program, "diffColor");
    glUniform3fv(loc, 1, diffuseColor);
    if (ambientColor.x() < 0) {
        ambientColor = 0.15f * diffuseColor;
    }
    loc = glGetUniformLocation(active_program, "ambientColor");
    glUniform3fv(loc, 1, ambientColor);
    loc = glGetUniformLocation(active_program, "specColor");
    glUniform3fv(loc, 1, specularColor);
    loc = glGetUniformLocation(active_program, "shininess");
    glUniform1f(loc, shininess);
    loc = glGetUniformLocation(active_program, "alpha");
    glUniform1f(loc, alpha);
}

void GLProgram::updateLight(Vector3f pos, Vector3f color) const {
    int loc = glGetUniformLocation(active_program, "lightPos");
    glUniform3fv(loc, 1, pos);

    loc = glGetUniformLocation(active_program, "lightDiff");
    glUniform3fv(loc, 1, color);
}


