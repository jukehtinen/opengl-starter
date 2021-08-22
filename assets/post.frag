#version 450 core

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D texAO;
layout(binding = 2) uniform sampler2D texBloom;

layout(location = 0) in vec2 Uv;

uniform bool enableAo = true;
uniform bool visualizeAo = false;
uniform bool enableBloom = true;
uniform bool visualizeBloom = false;

out vec4 outColor;

void main() 
{
	outColor = texture(texSampler, Uv);

	if (enableBloom)
		outColor += texture(texBloom, Uv);

	if (enableAo)
		outColor *= texture(texAO, Uv).x;

	if (visualizeAo) 
	{
		float ao = texture(texAO, Uv).x;
		outColor = vec4(ao, ao, ao, 1.0);
	}

	if (visualizeBloom) 
	{
		outColor = texture(texBloom, Uv);
	}

	outColor.a = 1.0;
}