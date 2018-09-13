#version 100

precision highp float;

varying vec2 texcoord;
varying vec3 color;

uniform sampler2D texture_diffuse;

void main()
{
        gl_FragColor = texture2D(texture_diffuse, texcoord) * vec4(color, 1.0);
}
