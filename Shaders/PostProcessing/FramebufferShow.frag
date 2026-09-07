#version 430 core

in vec2 texCoord;
uniform sampler2D screenTexture;
out vec4 color_out;

void main(void)
{
    color_out = texture(screenTexture, texCoord);
}
