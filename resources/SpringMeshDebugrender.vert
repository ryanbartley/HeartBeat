#version 150 core

in vec3 position;

uniform mat4 ciModelViewProjection;

void main(void)
{
	gl_Position = ciModelViewProjection * vec4(position, 1.0);
}
