#version 330
// fragment shader for phong-model lighting with a single directional light source.
// TODO: extend this shader to
//  - support diffuse textures
//  - support shadow mapping

in vec4 var_Color;
in vec3 var_Normal;
in vec3 var_Position;

uniform vec3 camPos;

uniform vec3 diffColor;
uniform vec3 specColor;
uniform vec3 ambientColor;
uniform float shininess;
uniform float alpha;

// TODO add more uniforms

uniform sampler2D diffuseTex;

uniform vec3 lightPos;
uniform vec3 lightDiff;

layout(location=0) out vec4 out_Color;

vec4 blinn_phong(vec3 kd) {
    // Implement Blinn-Phong Shading Model
    // 1. Convert everything to world space
    //    and normalize directions
    vec4 pos_world = vec4(var_Position, 1);
    vec3 normal_world = normalize(var_Normal);
    pos_world /= pos_world.w;
    vec3 light_dir = normalize(lightPos);
    vec3 cam_dir = camPos - pos_world.xyz;
    cam_dir = normalize(cam_dir);

    // 2. Compute Diffuse Contribution
    float ndotl = max(dot(normal_world, light_dir), 0.0);
    vec3 diffContrib = lightDiff * kd * ndotl;

    // 3. Compute Specular Contribution
    vec3 R = reflect( -light_dir, normal_world );
    float eyedotr = max(dot(cam_dir, R), 0.0);
    vec3 specContrib = pow(eyedotr, shininess) *
                       specColor * lightDiff;

    // 4. Add specular and diffuse contributions
    return  + vec4(diffContrib + specContrib, alpha);
}

void main () {
	// TODO implement texture mapping here
	// TODO implement shadow mapping here

    vec3 kd = diffColor;
    kd = texture(diffuseTex, var_Color.xy).xyz;
    out_Color = vec4(ambientColor + blinn_phong(kd).xyz, 1);
    // Uncomment following line to debug textures.
    // out_Color = vec4(var_Color.xy, 0, 0);
}
