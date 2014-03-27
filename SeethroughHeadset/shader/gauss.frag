#version 150

in vec2 _textureCoord;
in vec2 v_blurTexCoords[14];

out vec4 outputF;

uniform sampler2D tex;

void main(void)
{

	vec4 f = vec4(0.0);

    f += texture2D(tex, v_blurTexCoords[ 0])*0.0044299121055113265;
    f += texture2D(tex, v_blurTexCoords[ 1])*0.00895781211794;
    f += texture2D(tex, v_blurTexCoords[ 2])*0.0215963866053;
    f += texture2D(tex, v_blurTexCoords[ 3])*0.0443683338718;
    f += texture2D(tex, v_blurTexCoords[ 4])*0.0776744219933;
    f += texture2D(tex, v_blurTexCoords[ 5])*0.115876621105;
    f += texture2D(tex, v_blurTexCoords[ 6])*0.147308056121;
    f += texture2D(tex, _textureCoord        )*0.159576912161;
    f += texture2D(tex, v_blurTexCoords[ 7])*0.147308056121;
    f += texture2D(tex, v_blurTexCoords[ 8])*0.115876621105;
    f += texture2D(tex, v_blurTexCoords[ 9])*0.0776744219933;
    f += texture2D(tex, v_blurTexCoords[10])*0.0443683338718;
    f += texture2D(tex, v_blurTexCoords[11])*0.0215963866053;
    f += texture2D(tex, v_blurTexCoords[12])*0.00895781211794;
    f += texture2D(tex, v_blurTexCoords[13])*0.0044299121055113265;

	outputF = f;
}