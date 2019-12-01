#version 330 core

layout (location=0) in vec3 in_pos;
layout (location=1) in vec2 in_uv;
layout (location=2) in vec3 in_norm;
layout (location=3) in vec3 in_tan;
layout (location=4) in vec3 in_bitan;

uniform mat4 transform;
uniform mat4 model;
uniform mat4 lightSpace;

uniform vec3 lightPos;
uniform vec3 viewPos;

out VS_OUT {
	vec3 fragPos;
	vec3 norm;
	vec2 uv;
	vec3 tangent_lightPos;
	vec3 tangent_viewPos;
	vec3 tangent_fragPos;
	vec4 light_space_fragPos;
} vs_out;


void main(void)
{
	vs_out.fragPos = vec3(model * vec4(in_pos, 1.0));
	vs_out.uv = in_uv;
	vs_out.norm = vec3(model * vec4(in_norm, 0.0));

	mat3 normalMatrix = transpose(inverse(mat3(model)));
	vec3 T = normalize(normalMatrix * in_tan);
	vec3 N = normalize(normalMatrix * in_norm);
	T = normalize(T - dot(T, N) * N); // Gram-Schmidt re-orthogonalize
	vec3 B = cross(N, T);

	mat3 TBN = transpose(mat3(T, B, N));
	vs_out.tangent_lightPos = TBN * lightPos;
	vs_out.tangent_viewPos  = TBN * viewPos;
	vs_out.tangent_fragPos  = TBN * vs_out.fragPos;

	vs_out.light_space_fragPos = lightSpace * vec4(vs_out.fragPos, 1.0);
	gl_Position = transform * model * vec4(in_pos, 1.0);
}
