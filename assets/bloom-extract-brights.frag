#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 Uv;

uniform float threshold  = 0.6;

out vec4 outColor;

void main() 
{	
	vec4 color = texture(texSampler, Uv);
	float value = max(color.r, max(color.g, color.b));
	if (value < threshold)
		color = vec4(0.0);
	outColor = color;
}