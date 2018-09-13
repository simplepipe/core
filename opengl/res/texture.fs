layout (location = 0) out vec4 frag_color;

in vec2 texcoord;
in vec3 color;

uniform sampler2D texture_diffuse;

void main()
{
        frag_color = texture(texture_diffuse, texcoord) * vec4(color, 1.0);
}
