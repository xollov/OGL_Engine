#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex;

uniform mat4 vp;
uniform mat4 model;

void main(void) {
    gl_Position = vp * model * vec4(pos, 1.0);
}
