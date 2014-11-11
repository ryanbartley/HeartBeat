#version 150 core
#extension all : warn

// This input vector contains the vertex position in xyz, and the
// mass of the vertex in w
in vec4 position_mass;
// This is the current velocity of the vertex
in vec3 velocity;
// This is our connection vector
in ivec4 connection;
// This is to use for the waves
in vec2 tex_coord;


// This is a TBO that will be bound to the same buffer as the
// position_mass input attribute
uniform samplerBuffer tex_position;

#define MAX_TOUCHES 5

uniform vec2 touchesBegan[MAX_TOUCHES];
uniform int	numTouchesBegan;
uniform vec2 touchesMoved[MAX_TOUCHES];
uniform int numTouchesMoved;

// The outputs of the vertex shader are the same as the inputs
out vec4 tf_position_mass;
out vec3 tf_velocity;
out vec3 tf_normal;

// A uniform to hold the timestep. The application can update this
uniform float t = 0.07;

uniform float elapsedSeconds;

// The global spring constant
uniform float k = 10.0;

// Gravity
const vec3 gravity = vec3(0.0, 0.0, 0.0);

const float size = 50.0 * 50.0;

// Global damping constant
uniform float c = 2.8;

// Spring resting length
uniform float rest_length = 0.88;

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

vec3 calcNormal( vec3 v0, vec3 v1, vec3 v2 )
{
	vec3 ab = v1 - v0;
	vec3 ac = v2 - v0;
	return normalize(cross(ab, ac));
}

bool containsPoint( vec2 touch, vec3 point, float range )
{
	return touch.x > point.x - range && touch.x < point.x + range && touch.y > point.y - range && touch.y < point.y + range;
}

void main(void)
{
	vec3 p = position_mass.xyz;		// p can be our position
	
	float m = position_mass.w;		// m is the mass of our vertex
	vec3 u = velocity;				// u is the initial velocity
	vec3 F = gravity * m - c * u;	// F is the force on the mass
	bool fixed_node = true;			// Becomes false when force is applied
	
	vec3 connectionPositions[4];
	
	for ( int i = 0; i < 4; ++i ) {
		if(connection[i] != -1) {
			// q is the position of the other vertex
			vec3 q = texelFetch(tex_position, connection[i]).xyz;
			vec3 d = q - p;
			float x = length(d);
			F += -k * (rest_length - x) * normalize(d); //+ vec3(xOffset, yOffset, 0);
			fixed_node = false;
			connectionPositions[i] = q;
		}
		else
			connectionPositions[i] = vec3( -1, -1, -1 );
	}
	
	vec3 normal;
	vec3 v0 = connectionPositions[0];
	vec3 v1 = connectionPositions[1];
	vec3 v2 = connectionPositions[2];
	vec3 v3 = connectionPositions[3];
	
	if ( v0[0] != -1 && v1[0] != -1 ) {
		normal += calcNormal( p, v0, v1 );
	}
	if ( v1[0] != -1 && v2[0] != -1 ) {
		normal += calcNormal( p, v1, v2 );
	}
	if ( v2[0] != -1 && v3[0] != -1 ) {
		normal += calcNormal( p, v2, v3 );
	}
	if ( v3[0] != -1 && v0[0] != -1 ) {
		normal += calcNormal( p, v3, v0 );
	}
	
	normal = normalize( normal );
	
	// If this is a fixed node, reset force to zero
	if(fixed_node) {
		F = vec3(0.0);
		m = 1;
	}
	else {
		bool found = false;
		for( int i = 0; i < MAX_TOUCHES && i < numTouchesBegan; i++ ) {
			if( containsPoint( touchesBegan[i], p, 10 ) ) {
				p = vec3( touchesBegan[i], -30 );
				found = true;
				break;
			}
		}
		if( ! found ) {
			for( int i = 0; i < MAX_TOUCHES && i < numTouchesMoved; i++ ) {
				if( containsPoint( touchesMoved[i], p, 10 ) ) {
					p = vec3( touchesMoved[i], -20 );
					break;
				}
			}
		}
	}
	
	// Accelleration due to force
	vec3 a = F / m;
	
	// Displacement
	vec3 s = u * t + 0.5 * a * t * t;
	
	// Final velocity
	vec3 v = u + a * t;
	
	// Constrain the absolute value of the displacement per step
	s = clamp( s, vec3(-25.0), vec3(25.0) );
	
	// Write the outputs
	tf_position_mass = vec4((p + s), m );
	tf_velocity = v;
	tf_normal = normal;
}
