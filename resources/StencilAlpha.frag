#version 330

in vec2 vTexCoord;


uniform float alphaThreshold;
uniform sampler2D uTex0;

out vec4 oColor;


void main() {
	
	vec4 color = texture( uTex0, vTexCoord );
	
	if ( alphaThreshold == color.a ) {
		discard;
	}
	else {
		oColor = color;
	}
}