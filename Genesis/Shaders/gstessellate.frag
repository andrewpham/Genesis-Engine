#version 430 core

flat in vec4 color;

out vec4 output_color;

void main(void)
{
	output_color = color;
}
