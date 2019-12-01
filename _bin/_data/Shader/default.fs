#version 330 core

in vec2 uv;

out vec4 out_frag;

uniform sampler2D albedo;

void main(void)
{
	vec4 texel = texture(albedo, uv);
	if(texel.a < 0.1)
		discard; // hard edge alpha

	out_frag = texel;
}