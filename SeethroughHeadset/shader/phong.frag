#version 150

in vec2 _textureCoord;
in vec3 _normal;
in vec3 _position; 
in vec4 _screenPos;

out vec4 outputF;

uniform sampler2D colormap;
uniform sampler2D normalmap;

uniform bool hasColormap;
uniform bool hasNormalmap;

uniform vec3 lightPos;
uniform vec3 cameraPos;

uniform vec3 ambient;
uniform vec3 diffuse;
uniform vec3 specular;
uniform float shininess;



mat3 getTbn()	//source: https://stackoverflow.com/questions/5255806/how-to-calculate-tangent-and-binormal
{
	// compute derivations of the world position
	vec3 p_dx = dFdx(_position);
	vec3 p_dy = dFdy(_position);
	// compute derivations of the texture coordinate
	vec2 tc_dx = dFdx(_textureCoord);
	vec2 tc_dy = dFdy(_textureCoord);
	// compute initial tangent and bi-tangent
	vec3 t = normalize( tc_dy.y * p_dx - tc_dx.y * p_dy );
	vec3 b = normalize( tc_dy.x * p_dx - tc_dx.x * p_dy ); // sign inversion
	// get new tangent from a given mesh normal
	vec3 n = normalize(_normal);
	vec3 x = cross(n, t);
	t = cross(x, n);
	t = normalize(t);
	// get updated bi-tangent
	x = cross(b, n);
	b = cross(n, x);
	b = normalize(b);
	mat3 tbn = transpose(mat3(t, b, n));
	return tbn;
}

void main(void)
{
	
	//ESSENTIAL VECTORS
	vec3 N = normalize(_normal);				//normal
	vec3 V = normalize(cameraPos - _position);	//view vector: pos->cam
	vec3 L = normalize(lightPos - _position);		//light vector: pos->light


	//transformed if there's a normal map
	vec3 NMap = N;
	vec3 VMap = V;
	vec3 LMap = L;

	//TEXTURES
		
	//colormap
	vec4 colormapVal = vec4(1.0,1.0,1.0,1.0);
	if(hasColormap){
		colormapVal = texture(colormap, _textureCoord);
	}
		

	//normalmap
	if(hasNormalmap){
		//vec4 normalmapVal = texture(normalmap, _textureCoord);
		//vec3 mapNormal = (vec3(normalmapVal)-0.5); //map from[0,1] to [-0.5,0.5]
		//N = normalize(N+mapNormal);

		mat3 tbn = getTbn();
		NMap = normalize( 2 * texture(normalmap, _textureCoord).rgb-1 );
		LMap = normalize(tbn * L);
		VMap = normalize(tbn * V);
	}



	//standard lighting:
	
	// Diffuse
	vec3 diff = diffuse * max(0.0, dot(NMap, LMap));
	
	// Specular
	vec3 R = normalize(reflect(-LMap, NMap));		//light reflection
	float cosinus = pow(max(dot(R, VMap), 0.0),shininess);
	vec3 spec = specular * cosinus;


	outputF = 0.1*vec4(ambient, 1.0)*colormapVal + 0.9*vec4(diff, 1.0)*colormapVal + vec4(spec, 1.0);

}