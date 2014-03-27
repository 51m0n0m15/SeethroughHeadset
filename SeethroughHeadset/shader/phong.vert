#version 150

in  vec4 position;
in  vec3 normal;
in	vec2 textureCoord;

out vec2 _textureCoord;
out vec3 _normal;
out vec3 _position;
out vec4 _screenPos;		//maybe needed later
out vec4 _texCoordProj;		//projected texture

uniform mat4 projMatrix;
uniform mat4 viewMatrix;
uniform mat4 modelMatrix;

void main(void)
{
	_normal = normalize(transpose(inverse(mat3(modelMatrix))) * normal); // Phong, "fake" normal transformation
	_position = vec3(modelMatrix * position);	//vec3(viewMatrix * modelMatrix * position); // Don't include projection here - we only want to go to view coordinates
	_textureCoord = textureCoord;
	_screenPos = projMatrix * viewMatrix * modelMatrix * position;
	gl_Position = _screenPos;
}
