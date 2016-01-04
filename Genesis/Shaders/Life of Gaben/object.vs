#version 430 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 texCoords;

out vec3 Normal;
out vec3 FragPos;
out vec2 TexCoords;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(position, 1.0f);
	// Calculate normal in world space
    Normal = mat3(transpose(inverse(model))) * normal; 
	// Calculate fragment position in world space
	FragPos = vec3(model * vec4(position, 1.0f));
    TexCoords = texCoords;
}