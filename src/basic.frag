#version 150 core

in vec2 vTexCoord;
in vec4 vColor;

out vec4 fragColor;

uniform int useTex;
uniform sampler2D tex;

void main() 
{
    vec4 texColor = vec4(1, 1, 1, 1);
    if (useTex == 1)
    {
        texColor = texture(tex, vTexCoord);
    }

    texColor *= vColor;

    if (texColor.a == 0)
    {
        discard;
    }

    fragColor = texColor;
}