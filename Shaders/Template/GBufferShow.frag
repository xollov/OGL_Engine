#version 430 core

in vec2 texCoord;
uniform usampler2D buff;
out vec4 color_out;

struct frag_info {
    vec3 ws;
    vec3 norm;
    vec2 tex;
};

void getFragInfo(uvec4 data, out frag_info info) {

    vec2 temp = unpackHalf2x16(data.x);
    info.ws.xy = temp;

    temp = unpackHalf2x16(data.y);
    info.ws.z = temp.x;
    info.norm.x = temp.y;

    temp = unpackHalf2x16(data.z);
    info.norm.yz = temp;

    temp = unpackHalf2x16(data.w);
    info.tex = temp;
}
uniform uint vis_mode;
void main(void)
{
    frag_info info;
    uvec4 data = texelFetch(buff, ivec2(gl_FragCoord.xy), 0);
    getFragInfo(data, info);

    switch(vis_mode) {
        default:
        case 0:
            color_out = vec4(info.ws, 1);
            break;
        case 1:
            color_out = vec4(info.norm, 1);
            break;
        case 2:
            color_out = vec4(info.tex, 0, 1);
            break;
    }
}
