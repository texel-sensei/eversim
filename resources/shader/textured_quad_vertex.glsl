#version 440

layout(location = 0) in vec2 in_Position;
layout(location = 1) in vec2 in_UV;

struct instanced_entity_information {
	mat4 M;
};

layout(std430,binding=42) buffer InstanceData
{
	instanced_entity_information data[];
};

//Uniforms
//uniform mat3 M;
uniform mat3 V;
uniform mat3 P;

//uniform vec2 texoffset;
//uniform vec2 texsize;
//uniform vec2 spritesize;

out vec2 ex_UV;

void main(void)
{
	instanced_entity_information instance = data[gl_InstanceID];
	mat4 instance_values = instance.M;
	
	mat3 M = mat3(instance_values);	
	
	vec2 texoffset = vec2(instance_values[0].w,instance_values[1].w);
	vec2 texsize = vec2(instance_values[2].w,instance_values[3].w);
	vec2 spritesize = instance.M[3].xy;
	
	mat3 MV = V * M;
	mat3 MVP = P * V * M;
	gl_Position = vec4(MVP * vec3(in_Position,1),1);
	
	ex_UV = texoffset + texsize * in_UV;
	ex_UV /= spritesize;
	
	//ex_UV = in_UV;
}