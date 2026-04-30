#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex;

uniform mat4 vp;
uniform mat4 model;

out VERT {
    vec3 position;
    vec3 normal;
    vec2 tex;
} vs_out;

void main(void) {

    vs_out.position = vec3(model * vec4(pos,1.0));
    vs_out.normal = mat3(transpose(inverse(model))) * norm;
    vs_out.tex = tex;

    gl_Position = vp * model * vec4(pos, 1.0);;
}
