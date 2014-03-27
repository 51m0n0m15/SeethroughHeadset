#version 330 core

in  vec4 position;
in	vec2 textureCoord;
out vec2 _textureCoord;

//uniform int lr;

void main(){
	gl_Position =  position;
	//if(lr==0) _textureCoord = (position.xy+vec2(1,1))/vec2(1,2);
	//if(lr==1) _textureCoord = (position.xy+vec2(0,1))/vec2(1,2);
	_textureCoord = (position.xy+vec2(1,1))/2.0;
}

