#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texcoord;

uniform vec2 window;
uniform vec2 size; //Texture size
uniform vec2 position;

vec2 allign(vec2 v,vec2 s)
{
	v /= s;
	v *= 2.0;
	v -= vec2(1.0);
	return v;
}

void main() 
{
	float tw = size.x;
	float th = size.y;
	
	vec2 ur = allign(position+vec2(tw,th),window);
	vec2 ul = allign(position+vec2(0,th),window);
	vec2 dr = allign(position+vec2(tw,0),window);
	vec2 dl = allign(position+vec2(0,0),window);
	
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
}