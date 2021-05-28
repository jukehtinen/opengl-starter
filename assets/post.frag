#version 450 core

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D texAO;

layout(location = 0) in vec2 Uv;

uniform bool enableAo = true;
uniform bool visualizeAo = false;

out vec4 outColor;

void main() 
{
	outColor = texture(texSampler, Uv);

	if (enableAo)
		outColor *= texture(texAO, Uv).x;

	if (visualizeAo) {
		float ao = texture(texAO, Uv).x;
		outColor = vec4(ao, ao, ao, 1.0);
	}

	outColor.a = 1.0;
}