#version 330

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT
{
	vec2 texCoord;
	vec3 position;
} gs_in[3];

out GS_OUT
{
	vec3 position;
	vec2 texCoord;
	vec3 normal;
} gs_out;

uniform mat3 ciNormalMatrix;
uniform mat4 ciModelView;

vec3 calcNormal( vec3 v0, vec3 v1, vec3 v2 )
{
	vec3 ab = v1 - v0;
	vec3 ac = v2 - v0;
	return normalize(cross(ab, ac));
}

void main() {
	vec3 v0 = gl_in[0].gl_Position.xyz;
	vec3 v1 = gl_in[1].gl_Position.xyz;
	vec3 v2 = gl_in[2].gl_Position.xyz;
	vec3 normal0 = calcNormal( v0, v1, v2 );
	vec3 normal1 = calcNormal( v1, v2, v0 );
	vec3 normal2 = calcNormal( v2, v0, v1 );
	
	gl_Position = gl_in[0].gl_Position;
	gs_out.normal = normalize( ciNormalMatrix * normal0 );
	gs_out.texCoord = gs_in[0].texCoord;
	gs_out.position = vec3( ciModelView * vec4( gs_in[0].position, 1.0 ) );
	EmitVertex();
	
	gl_Position = gl_in[1].gl_Position;
	gs_out.normal = normalize( ciNormalMatrix * normal1 );
	gs_out.texCoord = gs_in[1].texCoord;
	gs_out.position = vec3( ciModelView * vec4( gs_in[1].position, 1.0 ) );
	EmitVertex();
	
	gl_Position = gl_in[2].gl_Position;
	gs_out.normal = normalize( ciNormalMatrix * normal2 );
	gs_out.texCoord = gs_in[2].texCoord;
	gs_out.position = vec3( ciModelView * vec4( gs_in[2].position, 1.0 ) );
	EmitVertex();
	
	EndPrimitive();
}