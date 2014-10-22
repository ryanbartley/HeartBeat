
uniform sampler2D tex0;
uniform float diffuseBase;
uniform vec3  diffuseAdjust;
uniform float specularContrib;
uniform float specularPower;
uniform vec3  specularColor;
uniform float time;

varying vec3	P;
varying vec3	N;
varying vec3	V;		// View Direction (aka Eye Vector)
varying vec3	L;		// Light Direction
varying float	VdotN;
varying float	LdotN;

bool isNan( vec3 val )
{
	bool r = ( val.r <= 0.0 || 0.0 <= val.r ) ? false : true;
	bool g = ( val.g <= 0.0 || 0.0 <= val.g ) ? false : true;
	bool b = ( val.b <= 0.0 || 0.0 <= val.b ) ? false : true;
	return r && g && b;
}

void main()
{
	vec2 uv = gl_TexCoord[0].xy;
	float u1 = uv.x + 0.0025*((sin( 32*uv.y  + 0.75*time ) + 1.0)/2.0);
	float v1 = uv.y + 0.0025*((sin( 24*uv.y  + 0.65*time ) + 1.0)/2.0);
	vec4 samp = texture2D( tex0, vec2( u1, v1 ) );

	float D = diffuseBase + (1.0 - diffuseBase)*abs(LdotN);

	vec3 H = normalize( L + V );
	float specVal = abs( dot( N, H ) );
	float S = pow( specVal, specularPower );

	vec3 rgb = samp.rgb + diffuseAdjust;
	rgb = D*rgb + specularContrib*S*specularColor;
	gl_FragColor = vec4( rgb, 1 );

	float s = 1.0 - max( 0.0, dot( N, vec3( 0, 0, 1 ) ) );
	s = max( s, 0 );
	s = pow( s, 2.0 );
	if( ! isNan( N ) ) {
		gl_FragColor.rgb += 0.1*s;
	}
}
