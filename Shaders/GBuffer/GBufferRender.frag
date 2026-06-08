#version 430 core

layout (binding = 0) uniform usampler2D buff0;
layout (binding = 1) uniform sampler2D buff1;
layout (binding = 2) uniform usampler2D buff2;

struct frag_info {
    vec3 ws;
    vec3 norm;
    vec2 tex;
    vec3 diffuse;
    uint matID;
};

struct Material {
    vec4 diffuse;
    vec4 specular;
    float shininess;
};

layout (binding = 0, std430) buffer material_buffer {
    Material materials[];
};

struct Light {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};
layout (binding = 1, std430) buffer light_buffer {
    Light lights[];
};


void getFragInfo(ivec2 coord, out frag_info info) {
    
    uvec4 data = texelFetch(buff0, coord, 0);
    vec2 temp = unpackHalf2x16(data.y);
    info.ws.xy = unpackHalf2x16(data.x);
    info.ws.z = temp.x;

    info.norm.x = temp.y;
    info.norm.yz = unpackHalf2x16(data.z);
    info.tex = unpackHalf2x16(data.w);
    
    info.diffuse = texelFetch(buff1, ivec2(gl_FragCoord.xy), 0).rgb;
    info.matID = texelFetch(buff2, coord, 0).r;
}

uniform vec3 viewPosition;
uniform uint matID;
uniform uint lightCount;

vec4 calculateLighting() {

    vec3 result = vec3(0);
    frag_info info;
    getFragInfo(ivec2(gl_FragCoord.xy), info);
    Material material = materials[info.matID];

    //return vec4(info.diffuse, 1);

    for (int i = 0; i < lightCount; i++) {
        Light light = lights[i];

        vec3 ambient = light.ambient.rgb * material.diffuse.rgb * info.diffuse;

        vec3 normal = normalize(info.norm);

        // diffuse color
        vec3 lightDir = normalize(light.position.xyz - info.ws);
        float diff = max(dot(normal, lightDir), 0.0f); 
        vec3 diffuse = light.diffuse.rgb * diff * material.diffuse.rgb * info.diffuse;

        // specular color
        vec3 viewDir = normalize(viewPosition - info.ws);
        vec3 reflectDir = reflect(-lightDir, normal);
        float spec = pow( max(dot(viewDir, reflectDir), 0.0f), 32);
        // TODO: specular map impl?
        // vec3 specular = light.specular.rgb * spec * texture(specularMap, texCoord).rgb;
        vec3 specular = light.specular.rgb * material.specular.rgb * spec;
        result += ambient + diffuse + specular;
    }

    return vec4(result, 1.0f);
}

in vec2 texCoord;
out vec4 color_out;
void main(void)
{
    color_out = calculateLighting();
}
