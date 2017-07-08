#version 330 core

uniform sampler2D tex;

in vec2 texcoord;
out vec3 FragColor;

void main()
{
	//gl_FragColor = texture(tex,texcoord);
	FragColor = texture(tex,texcoord).xyz;
}