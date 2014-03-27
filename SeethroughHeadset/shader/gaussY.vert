#version 330 core

in  vec4 position;

out vec2 _textureCoord;
out vec2 v_blurTexCoords[14];

uniform float blurSize;

void main(){
	gl_Position =  position;
	_textureCoord = (position.xy+vec2(1,1))/2.0;

	vec2 vMin = vec2(0.0,0.0);
	vec2 vMax = vec2(1.0,1.0);

	v_blurTexCoords[ 0] = clamp(_textureCoord + vec2(0.0, -7.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 1] = clamp(_textureCoord + vec2(0.0, -6.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 2] = clamp(_textureCoord + vec2(0.0, -5.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 3] = clamp(_textureCoord + vec2(0.0, -4.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 4] = clamp(_textureCoord + vec2(0.0, -3.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 5] = clamp(_textureCoord + vec2(0.0, -2.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 6] = clamp(_textureCoord + vec2(0.0, -1.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 7] = clamp(_textureCoord + vec2(0.0,  1.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 8] = clamp(_textureCoord + vec2(0.0,  2.0 * blurSize), vMin, vMax);
    v_blurTexCoords[ 9] = clamp(_textureCoord + vec2(0.0,  3.0 * blurSize), vMin, vMax);
    v_blurTexCoords[10] = clamp(_textureCoord + vec2(0.0,  4.0 * blurSize), vMin, vMax);
    v_blurTexCoords[11] = clamp(_textureCoord + vec2(0.0,  5.0 * blurSize), vMin, vMax);
    v_blurTexCoords[12] = clamp(_textureCoord + vec2(0.0,  6.0 * blurSize), vMin, vMax);
    v_blurTexCoords[13] = clamp(_textureCoord + vec2(0.0,  7.0 * blurSize), vMin, vMax);
}