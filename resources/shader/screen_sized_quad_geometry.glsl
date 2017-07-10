#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices = 4) out;

out vec2 texcoord;

uniform vec2 target_resolution;
uniform vec2 texture_size; //Texture size
uniform mat4 M;

vec2 allign(vec2 v,vec2 s)
{
	v /= s;
	v *= 2.0;
	v -= vec2(1.0);
	return v;
}

vec2 move(vec2 v)
{
	vec4 p = M * vec4(v,0,1);
	return p.xy/p.w;
}

void main() 
{
	float tw = texture_size.x;
	float th = texture_size.y;

	vec2 ur = allign( move(vec2(tw,th) ),target_resolution);
	vec2 ul = allign( move(vec2(0,th)),target_resolution);
	vec2 dr = allign( move(vec2(tw,0)),target_resolution);
	vec2 dl = allign( move(vec2(0,0)),target_resolution);
	
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