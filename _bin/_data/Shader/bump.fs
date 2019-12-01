#version 330 core

in VS_OUT {
	vec3 fragPos;
	vec3 norm;
    vec2 uv;
    vec3 tangent_lightPos;
    vec3 tangent_viewPos;
    vec3 tangent_fragPos;
	vec4 light_space_fragPos;
} fs_in;

uniform sampler2D albedoMap;
uniform sampler2D normalMap;
uniform sampler2D shadowMap;
uniform int bSpecular;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 out_frag;

float ShadowCalculation(vec4 fragPosLightSpace)
{
    vec3 projCoord = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoord = projCoord * 0.5 + 0.5; // transform to [0,1] range
	if(projCoord.z > 1.0)
		return 0.0;

	vec3 lightDir = lightPos - fs_in.fragPos;
	float bias = max(0.015 * (1.0 - dot(fs_in.norm, lightDir)), 0.0001);
    
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for(float x=-0.75; x<=0.75; x+=0.75)
	{
		for(float y=-0.75; y<=0.75; y+=0.75)
		{
			float pcfDepth = texture(shadowMap, projCoord.xy + vec2(x, y) * texelSize).r;
			shadow += projCoord.z - bias > pcfDepth ? 1.0 : 0.0;
		}
	} shadow /= 14.0;

	return shadow;
}

void main(void)
{
	vec4 color = texture(albedoMap, fs_in.uv);
	vec3 normal = normalize(texture(normalMap, fs_in.uv).rgb * 2.0 - 1.0);
	normal.r*=-1.0;

	if(color.a < 0.1)
		discard; // hard edge alpha

	// ambient - diffuse
	vec3 ambient = color.rgb * 0.3;
	vec3 lightDir = normalize(fs_in.tangent_lightPos - fs_in.tangent_fragPos);
	float diffuse = max(dot(lightDir, normal), 0.0); // tangent space
	// specular
	vec3 viewDir = normalize(fs_in.tangent_viewPos - fs_in.tangent_fragPos);
	vec3 halfDir = normalize(lightDir + viewDir);
	float spec = pow(max(dot(normal, halfDir), 0.0), 128.0);
	// shadow
	float shadow = ShadowCalculation(fs_in.light_space_fragPos);
	// fragment
	vec3 frag_color = ambient + (color.rgb * ((diffuse + spec) * (1.0 - shadow)) );
	out_frag = vec4(pow(frag_color.rgb, vec3(2.2)), color.a); // gamma correction
}
