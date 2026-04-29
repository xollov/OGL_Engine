#version 430 core

layout (location = 0) out uvec4 out_color;

in VERT {
    vec3 ws;
    vec3 norm;
    vec2 tex;
} fs_in;

void main(void) {
    uvec4 color = uvec4(0); 
    color.x = packHalf2x16(fs_in.ws.xy);
    color.y = packHalf2x16(vec2(fs_in.ws.z, fs_in.norm.x));
    color.z = packHalf2x16(fs_in.norm.yz);
    color.w = packHalf2x16(fs_in.tex);
    out_color = color;
}

