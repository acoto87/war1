#version 140

in vec3 vColor;
in vec2 vTexCoord;

out vec4 fragColor;

uniform sampler2D tex;

void main() { 
	fragColor = texture(tex, vTexCoord) * vec4(vColor, 1.0); 
}