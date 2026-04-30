#version 430 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;
layout (location = 2) in vec2 tex;

uniform mat4 vp;
uniform mat4 model;

out VERT {
    vec3 ws;
    vec3 norm;
    vec2 tex;
} vs_out;

void main(void) {
    gl_Position = vp * model * vec4(pos, 1);

    vs_out.ws = (model * vec4(pos, 1)).xyz; 
    vs_out.norm = mat3(model) * norm; 
    vs_out.tex = tex; 
}
    

