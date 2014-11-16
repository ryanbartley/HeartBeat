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


//
// Description : Array and textureless GLSL 2D/3D/4D simplex 
//               noise functions.
//      Author : Ian McEwan, Ashima Arts.
//  Maintainer : ijm
//     Lastmod : 20110822 (ijm)
//     License : Copyright (C) 2011 Ashima Arts. All rights reserved.
//               Distributed under the MIT License. See LICENSE file.
//               https://github.com/ashima/webgl-noise
// 

vec3 mod289(vec3 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 mod289(vec4 x) {
  return x - floor(x * (1.0 / 289.0)) * 289.0;
}

vec4 permute(vec4 x) {
     return mod289(((x*34.0)+1.0)*x);
}

vec4 taylorInvSqrt(vec4 r)
{
  return 1.79284291400159 - 0.85373472095314 * r;
}

float snoise(vec3 v)
  { 
  const vec2  C = vec2(1.0/6.0, 1.0/3.0) ;
  const vec4  D = vec4(0.0, 0.5, 1.0, 2.0);

// First corner
  vec3 i  = floor(v + dot(v, C.yyy) );
  vec3 x0 =   v - i + dot(i, C.xxx) ;

// Other corners
  vec3 g = step(x0.yzx, x0.xyz);
  vec3 l = 1.0 - g;
  vec3 i1 = min( g.xyz, l.zxy );
  vec3 i2 = max( g.xyz, l.zxy );

  //   x0 = x0 - 0.0 + 0.0 * C.xxx;
  //   x1 = x0 - i1  + 1.0 * C.xxx;
  //   x2 = x0 - i2  + 2.0 * C.xxx;
  //   x3 = x0 - 1.0 + 3.0 * C.xxx;
  vec3 x1 = x0 - i1 + C.xxx;
  vec3 x2 = x0 - i2 + C.yyy; // 2.0*C.x = 1/3 = C.y
  vec3 x3 = x0 - D.yyy;      // -1.0+3.0*C.x = -0.5 = -D.y

// Permutations
  i = mod289(i); 
  vec4 p = permute( permute( permute( 
             i.z + vec4(0.0, i1.z, i2.z, 1.0 ))
           + i.y + vec4(0.0, i1.y, i2.y, 1.0 )) 
           + i.x + vec4(0.0, i1.x, i2.x, 1.0 ));

// Gradients: 7x7 points over a square, mapped onto an octahedron.
// The ring size 17*17 = 289 is close to a multiple of 49 (49*6 = 294)
  float n_ = 0.142857142857; // 1.0/7.0
  vec3  ns = n_ * D.wyz - D.xzx;

  vec4 j = p - 49.0 * floor(p * ns.z * ns.z);  //  mod(p,7*7)

  vec4 x_ = floor(j * ns.z);
  vec4 y_ = floor(j - 7.0 * x_ );    // mod(j,N)

  vec4 x = x_ *ns.x + ns.yyyy;
  vec4 y = y_ *ns.x + ns.yyyy;
  vec4 h = 1.0 - abs(x) - abs(y);

  vec4 b0 = vec4( x.xy, y.xy );
  vec4 b1 = vec4( x.zw, y.zw );

  //vec4 s0 = vec4(lessThan(b0,0.0))*2.0 - 1.0;
  //vec4 s1 = vec4(lessThan(b1,0.0))*2.0 - 1.0;
  vec4 s0 = floor(b0)*2.0 + 1.0;
  vec4 s1 = floor(b1)*2.0 + 1.0;
  vec4 sh = -step(h, vec4(0.0));

  vec4 a0 = b0.xzyw + s0.xzyw*sh.xxyy ;
  vec4 a1 = b1.xzyw + s1.xzyw*sh.zzww ;

  vec3 p0 = vec3(a0.xy,h.x);
  vec3 p1 = vec3(a0.zw,h.y);
  vec3 p2 = vec3(a1.xy,h.z);
  vec3 p3 = vec3(a1.zw,h.w);

//Normalise gradients
  vec4 norm = taylorInvSqrt(vec4(dot(p0,p0), dot(p1,p1), dot(p2, p2), dot(p3,p3)));
  p0 *= norm.x;
  p1 *= norm.y;
  p2 *= norm.z;
  p3 *= norm.w;

// Mix final noise value
  vec4 m = max(0.6 - vec4(dot(x0,x0), dot(x1,x1), dot(x2,x2), dot(x3,x3)), 0.0);
  m = m * m;
  return 42.0 * dot( m*m, vec4( dot(p0,x0), dot(p1,x1), 
                                dot(p2,x2), dot(p3,x3) ) );
  }

// This is a TBO that will be bound to the same buffer as the
// position_mass input attribute
uniform samplerBuffer tex_position;

#define MAX_TOUCHES 8
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
uniform float elapsedSeconds;

// The global spring constant
uniform float k;

// Gravity
const vec3 gravity = vec3(0.0, 0.0, 0.0);

// Global damping constant
uniform float c;

// Spring resting length
uniform float rest_length;


const float pi = 3.14159;
uniform float waterHeight = 10;
uniform int numWaves = 3;
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
	return touch.x > point.x - range && 
			touch.x < point.x + range && 
			touch.y > point.y - range && 
			touch.y < point.y + range;
}

vec3 calcTouchAccel( vec3 pos,float m, vec3 touch, float dist, float maxAccel )
{
	vec3 diff = pos - touch;
	diff = normalize( diff );
	diff *=  pow( dist, 4.0 );
	diff *= -1.;
	return diff;
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
	//bool touched = false;
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
			if( dist < touchBeganDistThreshold ) {
				touchAccel += calcTouchAccel( p, m, vec3(touchesBegan[i], 200), dist, 800.0 );
				// touched = true;
				break;
			}
		}
		for( int i = 0; i < MAX_TOUCHES && i < localNumTouchesMoved; i++ ) {
			float dist = distance(vec3(touchesMoved[i], 0), p);
			if( dist < touchMovedDistThreshold) {
				touchAccel += calcTouchAccel( p, m, vec3(touchesMoved[i], 200), dist, 800.0 );
				// touched = true;
				break;
			}
		}
	}
	
	float wHeight = 0;

	// if( ! touched ) {
		// wHeight = waveHeight( p.x, p.y );
	// }
	//vec3  wNorm   = waveNormal( p.x, p.y );

	// Accelleration due to force
	vec3 a = (F + touchAccel ) / m;
	
	// Displacement
	vec3 s = u * t + 0.5 * a * t * t;
	
	// Final velocity
	vec3 v = u + a * t;
	
	v = clamp( v, vec3(-40.0), vec3(40.0) );
	// Constrain the absolute value of the displacement per step
	s = clamp( s, vec3(-0.5), vec3(0.5) );
	
	// Write the outputs
	tf_position_mass = vec4((p + s), m );
	if(fixed_node)tf_position_mass.xyz = p;
	tf_velocity = v;
	tf_normal = normal;
}
