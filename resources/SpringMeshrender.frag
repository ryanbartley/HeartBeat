#version 330

in VS_OUT
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} fs_in;

uniform sampler2D tex;

uniform vec4 lightPosition = vec4( 0, 0, 5, 1 );
uniform vec3 lightIntensity = vec3( 1.0 );
uniform vec3 Kd = vec3( 0.6, 0.6, 0.6 );	// Diffuse reflectivity
uniform vec3 Ka = vec3( 0.2, 0.2, 0.2 );	// Ambient reflectivity
uniform vec3 Ks = vec3( 1.0, 1.0, 1.0 );	// Specular reflectivity

uniform float shininess = 128.0;    // Specular shininess factor

out vec4 color;

vec3 ads( ) {
	vec3 n = normalize( fs_in.normal );
	vec3 s = normalize( vec3(lightPosition) - fs_in.position );
	vec3 v = normalize(vec3(-fs_in.position));
	vec3 r = reflect( -s, n );
	return
	lightIntensity *
	( Ka +
	 Kd * max( dot(s, n), 0.0 ) +
	 Ks * pow( max( dot(r,v), 0.0 ), shininess ) );
}
void main() {
	vec3 texColor = texture( tex, fs_in.texCoord ).xyz;
	color = vec4(ads() * texColor, 1.0);
}