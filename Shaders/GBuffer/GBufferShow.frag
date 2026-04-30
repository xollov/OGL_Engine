#version 430 core

in vec2 texCoord;
layout (binding = 0) uniform usampler2D buff0;
layout (binding = 1) uniform sampler2D buff1;
out vec4 color_out;

struct frag_info {
    vec4 diffuse;
    vec3 ws;
    vec3 norm;
    vec2 tex;
};

void getFragInfo(ivec2 coord, out frag_info info) {
    
    uvec4 data = texelFetch(buff0, coord, 0);
    vec2 temp = unpackHalf2x16(data.y);
    info.ws.xy = unpackHalf2x16(data.x);
    info.ws.z = temp.x;

    info.norm.x = temp.y;
    info.norm.yz = unpackHalf2x16(data.z);
    info.tex = unpackHalf2x16(data.w);
    
    info.diffuse = texelFetch(buff1, ivec2(gl_FragCoord), 0);
    
}
uniform uint vis_mode;
void main(void)
{
    frag_info info;
    getFragInfo(ivec2(gl_FragCoord.xy), info);

    switch (vis_mode) {
        default:
        case 1:
            color_out = info.diffuse;
            break;
        case 2:
            color_out = vec4(info.ws, 1);
            break;
        case 3:
            color_out = vec4(info.norm, 1);
            break;
        case 4:
            color_out = vec4(vec3(info.tex, 1), 1);
            break;
    }
}
