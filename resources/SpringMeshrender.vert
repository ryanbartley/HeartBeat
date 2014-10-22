#version 330

in vec3 position;
in vec2 texCoord;

uniform mat4 ciModelViewProjection;

out VS_OUT
{
	vec2 texCoord;
	vec3 originalPosition;
} vs_out;

void main() {
	vs_out.originalPosition = position;
	vs_out.texCoord = texCoord;
	gl_Position = ciModelViewProjection * vec4( position, 1.0 );
}