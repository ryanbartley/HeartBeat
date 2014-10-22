#version 330

in GS_OUT
{
	vec2 texCoord;
	vec3 normal;
} fs_in;

uniform mat4 ciModelView;
uniform mat4 ciNormalMatrix;
uniform vec3 lightPosition = vec3( 0, 5, 5 );

uniform sampler2D tex;

out vec4 color;

void main() {
	vec2 uv = fs_in.texCoord;
	vec3 n = fs_in.normal;
	
	vec4 texColor = texture( tex, uv + -n.xy );
	
	
	
	color = vec4(n, 1);
}