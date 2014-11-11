#version 330

in vec3 position;
in vec3 normal;
in vec2 texCoord;

uniform mat4 ciModelViewProjection;
uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;

uniform float elapsedSeconds;

const float pi = 3.14159;
uniform float waterHeight = 10;
uniform int numWaves = 1;
uniform float amplitude[8];
uniform float wavelength[8];
uniform float speed[8];
uniform vec2 direction[8];

float wave(int i, float x, float y) {
	float frequency = 2*pi/wavelength[i];
	float phase = speed[i] * frequency;
	float theta = dot(direction[i], vec2(x, y));
	return amplitude[i] * sin(theta * frequency + elapsedSeconds * phase);
}

float waveHeight(float x, float y) {
	float height = 0.0;
	for (int i = 0; i < numWaves; ++i)
		height += wave(i, x, y);
	return height;
}

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