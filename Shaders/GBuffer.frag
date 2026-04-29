#version 430 core

layout (location = 0) out uvec4 out_color0;
layout (location = 1) out vec4 out_color1;

in VERT {
    vec3 ws;
    vec3 norm;
    vec2 tex;
} fs_in;

uniform uint material_id;
uniform sampler2D tex_diffuse;

void main(void) {
    uvec4 color0 = uvec4(0); 
    vec4 color1 = uvec4(0); 

    vec3 color = texture(tex_diffuse, fs_in.tex).rgb;

    color0.x = packHalf2x16(color.xy);
    color0.y = packHalf2x16(vec2(color.z, fs_in.norm.x));
    color0.z = packHalf2x16(fs_in.norm.yz);
    color0.w = material_id;

    color1.xyz = fs_in.ws;
    color1.w = 60.0;

    out_color0 = color0;
    out_color1 = color1;
}
