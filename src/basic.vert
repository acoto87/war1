#version 140

in vec2 position;
in vec3 color;
in vec2 texCoord;

out vec3 vColor;
out vec2 vTexCoord;

void main() 
{ 
	vColor = color;
	vTexCoord = texCoord;
	gl_Position = vec4(position, 0, 1); 
}