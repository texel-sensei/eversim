#version 440 core

uniform sampler2D tex;

in vec2 texcoord;
out vec4 FragColor;

void main()
{
	FragColor = texture(tex,texcoord);
}