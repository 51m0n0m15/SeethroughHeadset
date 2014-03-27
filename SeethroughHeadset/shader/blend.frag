#version 150

in vec2 _textureCoord;

out vec4 outputF;

uniform sampler2D tex1;
uniform sampler2D tex2;

void main(void)
{
	outputF = texture(tex1, _textureCoord) + texture(tex2, _textureCoord);
}