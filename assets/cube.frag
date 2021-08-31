#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in vec3 FragPosWs;
layout(location = 4) in vec3 NormalWs;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

uniform vec3 lightDir;

void main(void)
{
    vec3 norm = normalize(NormalWs);
    float diff = max(dot(norm, normalize(lightDir)), 0.0);
		
    outColor = texture(texSampler, Uv) * clamp(diff, 0.2, 1.0);
	outNormal = normalize(Normal);
}