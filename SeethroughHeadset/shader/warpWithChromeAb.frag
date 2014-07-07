#version 150

uniform sampler2D tex;
uniform vec2 LensCenter;
uniform vec2 Scale;
uniform vec2 ScaleIn;
uniform vec4 HmdWarpParam;
uniform vec4 ChromAbParam;

in vec2 _textureCoord;

out vec4 outputF;

void main(void)
{
	vec2 theta = (_textureCoord - LensCenter) * ScaleIn; // Scales texture coordinates to [-1, 1]
	float rSq = theta.x * theta.x + theta.y * theta.y;
	vec2 theta1 = theta * ( HmdWarpParam.x +
							HmdWarpParam.y * rSq +
							HmdWarpParam.z * rSq * rSq +
							HmdWarpParam.w * rSq * rSq * rSq);
	// Detect whether blue texture coordinates are out of range since these will scaled out the furthest.
	vec2 thetaBlue = theta1 * (ChromAbParam.z + ChromAbParam.w * rSq);
	vec2 tcBlue = LensCenter + Scale * thetaBlue;

	if (any(bvec2(clamp(tcBlue, vec2(0.0, 0.0), vec2(1.0, 1.0)) - tcBlue))) {
		outputF = vec4(0, 0, 0, 0);
		return;
	}

	// Now do blue texture lookup.
	float blue = texture2D(tex, tcBlue).b;
	// Do green lookup (no scaling).
	vec2 tcGreen = LensCenter + Scale * theta1;
	float green = texture2D(tex, tcGreen).g;
	// Do red scale and lookup.
	vec2 thetaRed = theta1 * (ChromAbParam.x + ChromAbParam.y * rSq);
	vec2 tcRed = LensCenter + Scale * thetaRed;
	float red = texture2D(tex, tcRed).r;
	
	outputF = vec4(red, green, blue, 1);
}
