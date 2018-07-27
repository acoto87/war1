#version 150 core

in vec2 position;
in vec2 texCoord;

out vec2 vTexCoord;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main() 
{ 
	vTexCoord = texCoord;
	gl_Position = proj * view * model * vec4(position, 0, 1); 
}