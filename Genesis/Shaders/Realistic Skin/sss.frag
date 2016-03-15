#version 430 core
out vec4 color;

in VS_OUT {
	vec3 Position;
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

layout(binding=0) uniform sampler2D texture_diffuse1;
// This is the Beckmann distribution texture
layout(binding=1) uniform sampler2D texture_specular1;
layout(binding=2) uniform sampler2D texture_ambient1;
layout(binding=3) uniform sampler2D texture_normal1;

uniform vec3 lightPos;
uniform vec3 viewPos;
uniform vec3 gauss1w = vec3(0.233, 0.455, 0.649);
uniform vec3 gauss2w = vec3(0.100, 0.336, 0.344);
uniform vec3 gauss3w = vec3(0.118, 0.198, 0.0);
uniform vec3 gauss4w = vec3(0.113, 0.007, 0.007);
uniform vec3 gauss5w = vec3(0.358, 0.004, 0.0);
uniform vec3 gauss6w = vec3(0.078, 0.0, 0.0);
uniform float GaussWidth = 1.0;
uniform float mix = 0.5;
uniform bool profile1On = false;
uniform bool profile2On = false;
uniform bool profile3On = false;
uniform bool profile4On = false;
uniform bool profile5On = false;
uniform bool profile6On = false;
uniform bool scatteringOn = false;

float fresnelReflectance(vec3 H, vec3 V, float F0)
{
	float base = 1.0 - dot(V, H);
	float exponential = pow(base, 5.0);
	return exponential + F0 * (1.0 - exponential);
}

float PHBeckmann(float ndoth, float m)
{
	float alpha = acos(ndoth);
	float ta = tan(alpha);
	float val = 1.0/(m * m * pow(ndoth, 4.0)) * exp(-(ta * ta)/(m * m));
	return val;
}

float KS_Skin_Specular(vec3 N, // Bumped surface normal  
   vec3 L, // Points to light  
   vec3 V, // Points to eye  
   float m,  // Roughness  
   float rho_s)  
{  
	float result = 0.0;  
	float ndotl = dot(N, L);  
	if(ndotl > 0.0)  
	{	  
		vec3 h = L + V; // Unnormalized half-way vector  
		vec3 H = normalize(h);  
		float ndoth = dot(N, H);  
		float r = texture(texture_specular1, vec2(ndoth, m)).r;
		float PH = pow(2.0 * r, 10.0);  
		float F = fresnelReflectance(H, V, 0.028);  
		float frSpec = max(PH * F / dot(h, h), 0);  
		result = ndotl * rho_s * frSpec; // BRDF * dot(N, L) * rho_s 
	}  
	return result;  
}

vec4 convolveU()  
{  
	float scaleConv = 1.0 / 1024.0;
	float netFilterWidth = scaleConv * GaussWidth;  
	// Gaussian curve – standard deviation of 1.0  
	float curve[7] = {0.006, 0.061, 0.242, 0.383, 0.242, 0.061, 0.006};  
	vec2 coords = fs_in.TexCoords - vec2(netFilterWidth * 3.0, 0.0);  
	vec4 sum = vec4(0.0, 0.0, 0.0, 0.0);  
	for(int i = 0; i < 7; i++)  
	{  
		vec4 tap = texture(texture_diffuse1, coords);  
		sum += curve[i] * tap;  
		coords += vec2(netFilterWidth, 0.0);  
	}  
	return sum;  
}  

vec4 finalSkinShader()  
{  
	// The total diffuse light exiting the surface  
	vec3 diffuseLight = vec3(0.0, 0.0, 0.0);
	vec4 irrad1tap = texture(texture_diffuse1, fs_in.TexCoords);
	if (profile1On)
		diffuseLight += gauss1w * irrad1tap.xyz;
	if (profile2On)
		diffuseLight += gauss2w * irrad1tap.xyz;
	if (profile3On)
		diffuseLight += gauss3w * irrad1tap.xyz;
	if (profile4On)
		diffuseLight += gauss4w * irrad1tap.xyz;
	if (profile5On)
		diffuseLight += gauss5w * irrad1tap.xyz;
	if (profile6On)
		diffuseLight += gauss6w * irrad1tap.xyz;
	// Renormalize diffusion profiles to white  
	vec3 normConst = gauss1w + gauss2w + gauss3w + gauss4w + gauss5w + gauss6w;  
	diffuseLight /= normConst; // Renormalize to white diffuse light  
	// Four average thicknesses through the object (in mm)  
	vec4 thickness_mm =	1.0 * -(1.0 / 0.2) *  
						log(vec4(irrad1tap.w, irrad1tap.w,  
									irrad1tap.w, irrad1tap.w));
	vec4 a_values = vec4(0.433, 0.753, 1.412, 2.722);  
	vec4 inv_a = -1.0 / ( 2.0 * a_values * a_values );  
	vec4 fades = exp(thickness_mm * thickness_mm * inv_a); 
	if (profile1On)
		diffuseLight += gauss1w / normConst * fades.y * texture(texture_diffuse1, fs_in.TexCoords).xyz;  
	if (profile2On)
		diffuseLight += gauss2w / normConst * fades.y * texture(texture_diffuse1, fs_in.TexCoords).xyz;  
	if (profile3On)
		diffuseLight += gauss3w / normConst * fades.y * texture(texture_diffuse1, fs_in.TexCoords).xyz;
	if (profile4On)
		diffuseLight += gauss4w / normConst * fades.y * texture(texture_diffuse1, fs_in.TexCoords).xyz;  
	if (profile5On)
		diffuseLight += gauss5w / normConst * fades.z * texture(texture_diffuse1, fs_in.TexCoords).xyz;  
	if (profile6On)
		diffuseLight += gauss6w / normConst * fades.w * texture(texture_diffuse1, fs_in.TexCoords).xyz;  
	// Determine skin color from a diffuseColor map  
	diffuseLight *= sqrt(texture(texture_diffuse1, fs_in.TexCoords).xyz);
	vec3 specularLight = vec3(0.0, 0.0, 0.0);  
	// Compute specular for each light
	vec3 N = texture(texture_normal1, fs_in.TexCoords).rgb;
	N = normalize(N * 2.0 - 1.0);  // this normal is in tangent space
    specularLight += vec3(0.0, 0.0, 0.0) *  
                KS_Skin_Specular(N, lightPos, viewPos, 0.4, 0.35);  
	return vec4(diffuseLight + specularLight, 1.0);  
} 

void main()
{    
	if (scatteringOn)
		color = finalSkinShader();
	else
		color = vec4(texture(texture_diffuse1, fs_in.TexCoords));
}