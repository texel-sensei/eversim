#version 440

uniform sampler2D tex;
uniform vec2 texoffset;
uniform vec2 texsize;

in vec2 ex_UV;

void main(void)
{
	gl_FragColor = texture(tex,ex_UV);
}