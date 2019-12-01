#version 330 core

in VS_OUT
{
	vec4 line_color;
} fs_in;

out vec4 out_frag;



void main(void)
{
	out_frag = fs_in.line_color;
}