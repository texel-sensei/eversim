#version 440

layout(location=0) in vec3 in_Position;

//Uniforms
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;

void main(void)
{
	mat4 MV = V * M;
	mat4 MVP = P * V * M;
	gl_Position = MVP * vec4(in_Position,1);
}