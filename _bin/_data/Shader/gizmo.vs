#version 330 core

layout (location=0) in vec3 in_pos;

uniform mat4 transform;
uniform vec4 obj_color;

out VS_OUT {
	vec4 line_color;
} vs_out;

void main(void)
{
	vs_out.line_color = obj_color;
	
	gl_Position = transform * vec4(in_pos, 1.0);
}