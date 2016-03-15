#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out VS_OUT {
	vec3 Position;
	vec3 Normal;
	vec3 FragPos;
	vec2 TexCoords;
} vs_out;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	vs_out.Position = position;
	mat3 normalMatrix = mat3(transpose(inverse(model))); 
	vs_out.Normal = normalMatrix * normal;
	// Calculate fragment position in world space
	vs_out.FragPos = vec3(model * vec4(position, 1.0f));
    vs_out.TexCoords = texCoords;
}