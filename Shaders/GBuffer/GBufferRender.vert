#version 430 core

layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 tex;

out vec2 texCoord;

uniform mat4 m;

void main(void) {

    gl_Position = m * vec4(pos, 1);
    texCoord = tex;
}
