#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;

out VS_OUT
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} vs_out;

void main() {
	vs_out.position = vec3( ciModelView * vec4( position, 1.0 ) );
	vs_out.normal = normalize( ciNormalMatrix * normal );
	vs_out.texCoord = texCoord;
	gl_Position = ciModelViewProjection * vec4( position, 1.0 );
}