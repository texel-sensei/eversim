#version 440

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_UV;

//Uniforms
uniform mat3 M;
uniform mat3 V;
uniform mat3 P;

out vec2 ex_UV;

void main(void)
{
	mat3 MV = V * M;
	mat3 MVP = P * V * M;
	gl_Position = vec4(MVP * vec3(in_Position,1),1);
	ex_UV = in_UV;
}