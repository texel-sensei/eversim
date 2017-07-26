#version 440

uniform sampler2D tex;

in vec2 ex_UV;

void main(void)
{
	gl_FragColor = texture(tex,ex_UV);
}