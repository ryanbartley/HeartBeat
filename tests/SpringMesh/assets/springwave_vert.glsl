
uniform vec3 lightPos;

varying vec3	P;
varying vec3	N;
varying vec3	V;		// View Direction (aka Eye Vector)
varying vec3	L;		// Light Direction
varying float	VdotN;
varying float	LdotN;

void main()
{
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = gl_ModelViewProjectionMatrix*gl_Vertex;
	
	P = gl_Vertex.xyz;
	N = gl_Normal;
	N = normalize( N );
	L = -normalize( lightPos - P );

	VdotN = dot( V, N );
	LdotN = dot( L, N );
}
