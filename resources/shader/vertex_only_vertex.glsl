#version 440

layout(location=0) in vec3 in_Position;

out vec3 ex_Color;

//Uniforms
//uniform mat4 M;
//uniform mat4 V;
//uniform mat4 P;

void main(void)
{
	//mat4 MV = V * M;
	//mat4 MVP = P * V * M;
	//gl_Position = MVP * vec4(in_Position,1);
	gl_Position = vec4(in_Position,1);
}