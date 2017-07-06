#version 330 core

uniform sampler2D tex;

in vec2 texcoord;

void main()
{
	gl_FragColor = texture(tex,texcoord);
}