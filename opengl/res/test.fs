#version 300 es

precision highp float;

layout (location = 0) out vec4 frag_color_red;
layout (location = 1) out vec4 frag_color_blue;

in vec2 texcoord;
in vec3 color;

uniform sampler2D texture_diffuse;

void main()
{
        frag_color_red = mix(
                vec4(1.0, 0.0, 0.0, 1.0),
                texture(texture_diffuse, texcoord) * vec4(color, 1.0),
                0.5
        );
        frag_color_blue = mix(
                vec4(0.0, 0.0, 1.0, 1.0),
                texture(texture_diffuse, texcoord) * vec4(color, 1.0),
                0.5
        );
}
