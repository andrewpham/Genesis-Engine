#version 430 core
out vec4 FragColor;

in VS_OUT {
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} fs_in;

uniform sampler2D texture1;
uniform vec3 lightPos;
uniform vec3 viewPos;

void main()
{       
	vec3 color = texture(texture1, fs_in.TexCoords).rgb;
	// Ambient
    vec3 ambient = 0.05 * color;
    // Diffuse 
    vec3 lightDir = normalize(lightPos - fs_in.FragPos);
	vec3 normal = normalize(fs_in.Normal);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;
    // Specular
    vec3 viewDir = normalize(viewPos - fs_in.FragPos);
	vec3 halfwayDir = normalize(lightDir + viewDir);  
	float spec = pow(max(dot(normal, halfwayDir), 0.0), 32.0);
	vec3 specular = vec3(0.3) * spec; // assuming bright white light color
    FragColor = vec4(ambient + diffuse + specular, 1.0f);
}