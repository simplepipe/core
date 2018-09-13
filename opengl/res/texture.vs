#version 100

attribute vec3 a_pos;
attribute vec3 a_color;
attribute vec2 a_texcoord;

varying vec2 texcoord;
varying vec3 color;

void main()
{
        color = a_color;
        texcoord = a_texcoord;
        gl_Position = vec4(a_pos, 1.0);
}
