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
#define M_PI 3.1415926535897932384626433832795

uniform vec2	touchesBegan[MAX_TOUCHES];
uniform float	numTouchesBegan;
uniform vec2	touchesMoved[MAX_TOUCHES];
uniform float	numTouchesMoved;
uniform float	touchBeganDistThreshold;
uniform float	touchMovedDistThreshold;

// The outputs of the vertex shader are the same as the inputs
out vec4 tf_position_mass;
out vec3 tf_velocity;
out vec3 tf_normal;

// A uniform to hold the timestep. The application can update this
uniform float t; //= 0.07;

// The global spring constant
uniform float k = 30.0;

// Gravity
const vec3 gravity = vec3(0.0, 0.0, 0.0);

// Global damping constant
uniform float c =4.0;

// Spring resting length
uniform float rest_length = 0.88;

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

vec3 calcTouchAccel( vec3 pos, vec3 touch, float dist, float maxAccel )
{
	vec3 diff = pos - touch;
	
	diff = normalize( diff );
	diff *= pow( dist, 2.0 );
//	return clamp( diff, vec3(), vec3(maxAccel) );
	return clamp( diff, vec3( - maxAccel ), vec3( maxAccel ) );
//	return diff;
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
	
	vec3 touchAccel = vec3( 0 );
	
	// If this is a fixed node, reset force to zero
	if(fixed_node) {
		F = vec3(0.0);
		m = 1;
	}
	else {
		int localNumTouchesBegan = int(numTouchesBegan);
		int localNumTouchesMoved = int(numTouchesMoved);
		for( int i = 0; i < MAX_TOUCHES && i < localNumTouchesBegan; i++ ) {
			float dist = distance( vec3(touchesBegan[i], 0), p);
			if( dist < 70 ) {
				touchAccel += calcTouchAccel( p, vec3(touchesBegan[i], 100), dist, 600.0 );
				break;
			}
		}
		for( int i = 0; i < MAX_TOUCHES && i < localNumTouchesMoved; i++ ) {
			float dist = distance(vec3(touchesMoved[i], 0), p);
			if( dist < 30) {
				touchAccel += calcTouchAccel( p, vec3(touchesMoved[i], 100), dist, 600.0 );
				break;
			}
		}
	}
	
	// Accelleration due to force
	vec3 a = (F + touchAccel) / m;
	
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
