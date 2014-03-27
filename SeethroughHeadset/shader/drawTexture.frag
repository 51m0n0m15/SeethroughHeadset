#version 150

in vec2 _textureCoord;

out vec4 outputF;

uniform sampler2D tex;

void main(void)
{
	outputF = texture(tex, _textureCoord);
}