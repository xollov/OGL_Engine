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

uniform vec3 viewPosition;
uniform int matID;

in VERT {
    vec3 position;
    vec3 normal;
    vec2 tex;
} fs_in;

out vec4 Color;

vec3 calculateLight(int id) {

    Material material = materials[matID];
    Light light = lights[id];
    //ambient color
    vec3 ambient = light.ambient.rgb * material.diffuse.rgb;
    vec3 normal = normalize(fs_in.normal);

    // diffuse color
    vec3 lightDir = normalize(light.position.xyz - fs_in.position);
    float diff = max(dot(normal, lightDir), 0.0f); 
    vec3 diffuse = light.diffuse.rgb * diff * material.diffuse.rgb;

    // specular color
    vec3 viewDir = normalize(viewPosition - fs_in.position);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow( max(dot(viewDir, reflectDir), 0.0f), material.shininess);
    vec3 specular = light.specular.rgb * spec * material.specular.rgb;

    return ambient + specular;
}

void main(void) {

    vec3 color = vec3(0);
    for (int i = 0; i < 2; i++) {
        color += calculateLight(i);
    }
    Color = vec4(color, 1);
}
