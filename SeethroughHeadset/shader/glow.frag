#version 150

in vec2 _textureCoord;

out vec4 outputF;

uniform sampler2D glowmap;
uniform bool hasGlowmap;

void main(void)
{
	vec4 glowmapVal = vec4(0.0,0.0,0.0,1.0);
	if(hasGlowmap){
		glowmapVal = texture(glowmap, _textureCoord);
	}
	outputF = glowmapVal;
}