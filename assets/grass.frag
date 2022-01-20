#version 450 core

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec4 Color;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

void main(void)
{
    outColor = vec4(0.08, 0.30, 0.17, 1.0);
	outNormal = normalize(Normal);
}