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

uniform uint lightCount;
uniform vec3 viewPosition;

vec3 calculateLighting(frag_info frag) {
    vec3 result = vec3(0);
    Material mat = materials[frag.material_id];

    for (int i = 0; i < lightCount; i++) {
        Light light = lights[i];
        
        vec3 L = frag.ws_coord - light.position.xyz;
        float dist = length(L);
        L = normalize(L);

        vec3 N = normalize(frag.normal);
        vec3 R = reflect(-L, N);

        float NdotR = max(0.0, dot(N,R));
        float NdotL = max(0.0, dot(N,L));
        float attenuation = 50.0 / (pow(dist, 2.0) + 1.0);

        vec3 diffuse_color = light.diffuse.rgb * frag.color * NdotL *  attenuation;
        vec3 specular_color = light.specular.rgb * frag.color * pow(NdotR, mat.shininess) *  attenuation;
        
        result += diffuse_color + specular_color;
    }
    return result;
}

in vec2 texCoord;
out vec4 color_out;
void main(void)
{
    frag_info info;
    getFragInfo(ivec2(texCoord), info);
    color_out = vec4(calculateLighting(info), 1);
}
