#version 430 core

layout (location = 0) out uvec4 out_color0;
layout (location = 1) out vec4 out_color1;
layout (location = 2) out uint out_color2;

in VERT {
    vec3 ws;
    vec3 norm;
    vec2 tex;
} fs_in;

uniform sampler2D diffuse;
uniform uint matID;

void main(void) {

    uvec4 color0;
    color0.x = packHalf2x16(fs_in.ws.xy);
    color0.y = packHalf2x16(vec2(fs_in.ws.z, fs_in.norm.x));
    color0.z = packHalf2x16(fs_in.norm.yz);
    color0.w = packHalf2x16(fs_in.tex);

    out_color0 = color0; 
    out_color1 = texture(diffuse, fs_in.tex);
    out_color2 = matID; 
}

