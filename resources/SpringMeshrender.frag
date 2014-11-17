#version 330

in VS_OUT
{
	vec2 texCoord;
	vec3 position;
	vec3 normal;
} fs_in;

uniform sampler2D tex;

uniform mat4 ciModelView;

uniform vec3 lightPosition1 = vec3( 1500, -1500, 2000 );

uniform vec3 lightPosition2 = vec3( -1500, -1500, 2000 );
uniform vec3 lightPosition3 = vec3( 0,0, 2000 );
uniform vec3 lightPosition4 = vec3( 0, 1500, 2000 );

uniform vec3 lcolor1 = vec3( .6,.6,.6 );
uniform vec3 lcolor2 = vec3( .6,.6,.6 );
uniform vec3 lcolor3 = vec3( .6,.6,.6 );
uniform vec3 lcolor4 = vec3( .6,.6,.6 );


uniform vec3 lightIntensity;
uniform vec3 Kd = vec3( 0.6, 0.6, 0.6 );	// Diffuse reflectivity
uniform vec3 Ka = vec3( 0.2, 0.2, 0.2 );	// Ambient reflectivity
uniform vec3 Ks = vec3( 1.0, 1.0, 1.0 );	// Specular reflectivity

uniform float shininess = 128.0;    // Specular shininess factor


out vec4 color;

vec3 ads( in vec3 lightpos, in vec3 lightColor ) {
	vec3 n = normalize( fs_in.normal );
	vec3 s = normalize( lightpos - fs_in.position );
	vec3 v = normalize(vec3(-fs_in.position));
	vec3 r = reflect( -s, n );
	return
	lightIntensity *
	( Ka +
	 Kd * abs( dot(s, n) ) +
	 Ks * pow( abs( dot(r,v) ), shininess ) ) * lightColor;
}
void main() {
	vec3 n = normalize( fs_in.normal );
	vec3 v = normalize(vec3(-fs_in.position));

	vec3 texColor = texture( tex, fs_in.texCoord ).xyz;
	color = vec4( mix( vec3( 0.5, .7, 1. ), 
				 	( ads( lightPosition1, lcolor1 ) + 
				 	  ads( lightPosition2, lcolor2 ) +
				 	  ads( lightPosition3, lcolor3 ) +
				 	  ads( lightPosition4, lcolor4 ) ) * texColor, 
				 	max( dot( n, v ), 0.0 ) ), 1.0);
}