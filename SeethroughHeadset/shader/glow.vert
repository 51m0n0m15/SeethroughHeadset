#version 150

in  vec4 position;
in	vec2 textureCoord;

out vec2 _textureCoord;

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void)
{
	_textureCoord = textureCoord;
	gl_Position = projMatrix * viewMatrix * modelMatrix * position;
}
