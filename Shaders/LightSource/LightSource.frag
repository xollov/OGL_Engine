#version 430 core


struct Light {
    vec4 position;

    vec4 ambient;
    vec4 diffuse;
    vec4 specular;
};

layout (binding = 0, std430) buffer light_buffer {
    Light lights[];
};

uniform int lightID;
out vec4 Color;

void main(void) {
    Color = vec4(lights[lightID].diffuse.rgb, 1);
}
