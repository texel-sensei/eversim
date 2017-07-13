#version 440

in vec3 ex_Color;

void main(void)
{
	gl_FragColor = vec4(ex_Color,1);
}