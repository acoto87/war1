#version 150 core

in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D tex;

void main() 
{
	vec4 texColor = texture(tex, vTexCoord);
    
	if (texColor.a == 0)
	{
		discard;
	}
	
	fragColor = texColor;
}