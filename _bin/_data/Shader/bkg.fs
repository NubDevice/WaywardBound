#version 330 core

in vec2 uv;

out vec4 out_frag;

//uniform sampler2D albedo;

void main(void)
{
	//vec4 texel = texture(albedo, uv);
	//if(texel.a < 0.1)
	//	discard; // hard edge alpha

	if(uv.y<0.35) 
	{
		out_frag = vec4(0.0, 0.0, 0.0, 1.0); 
	}
	else
	{
	float h = max(0.0, 1.4-uv.y-pow(abs(uv.x-0.5), 3.0));
	out_frag.r = pow(h, 3.0);
	out_frag.g = pow(h, 7.0);
	out_frag.b = 0.2+pow(max(0.0, h-0.1), 10.0);
	out_frag.a = 1.0;
	}
}