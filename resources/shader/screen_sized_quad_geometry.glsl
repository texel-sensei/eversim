#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texcoord;

float window_width = 1920;
float window_height = 1080;
vec2 window = vec2(window_width,window_height);

int tw = 32;
int th = 32;

int x = 30;
int y = 60;

vec2 allign(vec2 v,vec2 size)
{
	v /= size;
	v *= 2.0;
	v -= vec2(1.0);
	return v;
}

void main() 
{
	vec2 ur = allign(vec2(x+tw,y+th),window);
	vec2 ul = allign(vec2(x,y+th),window);
	vec2 dr = allign(vec2(x+tw,y),window);
	vec2 dl = allign(vec2(x,y),window);
	
	gl_Position = vec4( ur, 0.5, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(ul, 0.5, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( dr, 0.5, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(dl, 0.5, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); 
	
	/*
	gl_Position = vec4( 1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 1.0 );
    EmitVertex();

    gl_Position = vec4(-1.0, 1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 1.0 ); 
    EmitVertex();

    gl_Position = vec4( 1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 1.0, 0.0 ); 
    EmitVertex();

    gl_Position = vec4(-1.0,-1.0, 0.5, 1.0 );
    texcoord = vec2( 0.0, 0.0 ); 
    EmitVertex();

    EndPrimitive(); */
}