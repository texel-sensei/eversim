#version 330 core

uniform sampler2D tex;

in vec2 texcoord;
out vec3 FragColor;

void main()
{
	FragColor = texture(tex,texcoord).xyz;
}