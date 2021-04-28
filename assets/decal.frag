#version 450 core

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D texDepth;

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 FragPos;
layout(location = 3) in vec3 inFragPosWs;

uniform mat4 invModel;
uniform mat4 invView;
uniform mat4 invProj;
uniform vec4 decalColor;

out vec4 outColor;

void main(void)
{
	vec2 normalized = FragPos.xy / FragPos.w;
	vec2 viewport = (normalized.xy + 1.0) / 2.0;

	float depth = texture(texDepth, viewport).r;

	vec3 clip = vec3(normalized, 2.0 * depth - 1.0);
	
	vec4 viewRaw = invProj * vec4(clip, 1.0);
    vec3 view = viewRaw.xyz / viewRaw.w;
	
	vec4 decal = invModel * invView * vec4(view, 1.0);
    decal = (decal + 1.0) / 2.0;

    if (decal.z < 0 || decal.x < 0 || decal.z > 1 || decal.x > 1 || decal.y < 0 || decal.y > 1) 
	{
		discard;
    } 
	else 
	{
        outColor = texture(texSampler, decal.xz) * decalColor;
    }
}