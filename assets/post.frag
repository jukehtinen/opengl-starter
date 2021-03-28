#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 Uv;

uniform float mixValue;

out vec4 outColor;

void main() 
{
	vec4 color = texture(texSampler, Uv);
	vec4 gray = vec4(vec3(dot(color.rgb, vec3(0.299, 0.587, 0.114))), 1.0);
	outColor = mix(color, gray, clamp(mixValue, 0.0, 1.0));
}