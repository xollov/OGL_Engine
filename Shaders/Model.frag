#version 430 core

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

uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;

uniform vec3 viewPosition;

in VERT {
    vec3 position;
    vec3 normal;
    vec2 tex;
} fs_in;
out vec4 Color;

vec3 calculateLight(int id) {

    Material material = materials[0];
    Light light = lights[id];
    //ambient color
    vec3 ambient = light.ambient.rgb * texture(diffuseMap, fs_in.tex).rgb;

    vec3 normal = normalize(fs_in.normal);
    //vec3 normal = texture(normalMap, fs_in.tex).rgb;
    //normal = normalize(normal);

    // diffuse color
    vec3 lightDir = normalize(light.position.xyz - fs_in.position);
    float diff = max(dot(normal, lightDir), 0.0f); 
    vec3 diffuse = light.diffuse.rgb * diff * texture(diffuseMap, fs_in.tex).rgb;

    // specular color
    vec3 viewDir = normalize(viewPosition - fs_in.position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow( max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * spec * texture(specularMap, fs_in.tex).rgb;
    return ambient + specular;
}

void main(void) {

    vec3 color = vec3(0);
    for (int i = 0; i < 2; i++) {
        color += calculateLight(i);
    }
    Color = vec4(color, 1);
}
