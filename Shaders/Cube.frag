#version 430 core

layout (location = 0) out vec4 color;

uniform sampler2D diffuse;

in vec2 texCoord;
void main(void) {
   color = texture(diffuse, texCoord); 
}
