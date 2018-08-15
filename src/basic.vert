#version 150 core

in vec2 position;
in vec2 texCoord;
in vec4 color;

out vec2 vTexCoord;
out vec4 vColor;

uniform vec2 viewSize;
uniform mat3 world;

void main() 
{ 
	vTexCoord = texCoord;
    vColor = color;

	// transform the position of the vertex to world coordinates
	vec3 worldPosition = world * vec3(position, 1.0);

	// transform  the position of the vertex with the ortographic projection matrix
	// this allow specify image dimensions in screen coordinates
    //
    // { 2/w,   0,  0, -1 } {x} = {(2/w) * x - 1}
    // {   0, 2/h,  0, -1 } {y} = {(2/h) * y - 1}
    // {   0,   0,  0,  0 } {z} = {0}
    // {   0,   0,  0,  1 } {1} = {1}
    //
	gl_Position = vec4(2.0 * worldPosition.x / viewSize.x - 1.0, 1.0 - 2.0 * worldPosition.y / viewSize.y, 0, 1);
}