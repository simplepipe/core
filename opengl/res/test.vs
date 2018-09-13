#version 300 es

layout (location = 0) in vec3 a_pos;
layout (location = 1) in vec3 a_color;
layout (location = 2) in vec2 a_texcoord;

out vec2 texcoord;
out vec3 color;

void main()
{
        color = a_color;
        texcoord = a_texcoord;
        gl_Position = vec4(a_pos, 1.0);
}
