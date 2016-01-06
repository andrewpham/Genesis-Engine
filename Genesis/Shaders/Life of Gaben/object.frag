#version 430 core
out vec4 FragColor;

in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
	vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform sampler2D texture1;
uniform sampler2D normalMap;

uniform vec3 lightPos;
uniform vec3 viewPos;

uniform bool hasNormalMap;

void main()
{       
	vec3 color = texture(texture1, fs_in.TexCoords).rgb;
	// Ambient
    vec3 ambient = 0.1 * color;
    // Diffuse 
	vec3 lightDir;
	vec3 normal;
	if (hasNormalMap)
	{
		lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
		normal = texture(normalMap, fs_in.TexCoords).rgb;
		normal = normalize(normal * 2.0 - 1.0);  // this normal is in tangent space
	}
	else
	{
		lightDir = normalize(lightPos - fs_in.FragPos);
		normal = normalize(fs_in.Normal);
	}
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
	vec3 viewDir;
	if (hasNormalMap)
	{
		viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
	}
	else
	{
		viewDir = normalize(viewPos - fs_in.FragPos);
	}
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.2) * spec; // assuming bright white light color
	
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}