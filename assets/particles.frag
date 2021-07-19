#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout (location = 0) in vec2 inUv;
layout (location = 1) in float inLife;
layout (location = 2) in vec4 inColor;

out vec4 outColor;

void main() 
{
	outColor = texture(texSampler, inUv) * inColor;
	//outColor.a *= inLife;
}