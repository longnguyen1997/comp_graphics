#version 330
// simple fragment shader for full-screen quad drawing

in vec4 var_Color;
in vec3 var_Normal;
in vec3 var_Position;

uniform sampler2D diffuseTex;

layout(location=0) out vec4 out_Color;
void main () {
    out_Color = vec4(texture(diffuseTex, var_Color.xy).xyz, 1);
}
