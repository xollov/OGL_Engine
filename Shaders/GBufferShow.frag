#version 430 core

uniform usampler2D buff0;
uniform sampler2D buff1;

struct Material {
    vec4 diffuse;
    vec4 specular;
    float shininess;
};
struct Light {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
struct frag_info {
    vec3 color;
    vec3 ws_coord;
    vec3 normal;
    uint material_id;
    float specular_power;
};

layout (binding = 0, std430) buffer material_buffer {
    Material materials[];
};

layout (binding = 1, std430) buffer light_buffer {
    Light lights[];
};

struct frag_info {
    vec3 color;
    vec3 ws_coord;
    vec3 normal;
    uint material_id;
    float specular_power;
};

void getFragInfo(ivec2 coord, out frag_info info) {

    uvec4 data0 = texelFetch(buff0, ivec2(coord),0);
    vec4 data1 = texelFetch(buff1, ivec2(coord), 0);
    vec2 temp;

    temp = unpackHalf2x16(data0.y);

    info.color = vec3(unpackHalf2x16(data0.x),temp.x);
    info.normal = vec3(temp.y, unpackHalf2x16(data0.z));
    info.material_id = data0.w;

    info.ws_coord = data1.xyz;
    info.specular_power = data1.w;
}

in vec2 texCoord;
uniform uint vis_mode = 0;

out vec4 color_out;

void main(void)
{
    frag_info info;
    getFragInfo(ivec2(texCoord), info);
    
    Material mat = materials[info.material_id];

    switch(vis_mode) {
        default:
        case 1:
            color_out = vec4(info.ws_coord, 1);
            break;
        case 2:
            color_out = vec4(info.normal, 1);
            break;
        case 3:
            vec3 color = vec3(0);
            color_out = vec4(mat.diffuse.rgb, 1);
            break;
        case 4:
            color_out = vec4(info.color, 1);
            break;
    }
}
