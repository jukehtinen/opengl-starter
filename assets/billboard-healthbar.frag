#version 450 core

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D texMask;

layout(location = 0) in vec2 Uv;

layout(location = 0) out vec4 outColor;

uniform float time;
uniform float health;
uniform float distanceToCamera;

vec2 frac(vec2 v)
{
	return v - floor(v);
}

void main(void)
{
	float mask = 1 - step(health, Uv.x);
	
	float blink = 1.0;
	if (health < 0.4)
		blink = sin(time * 8) * 0.2 + 0.8;
	
	vec4 borderMask = texture(texMask, Uv);
	
    vec3 color = texture(texSampler, vec2(health, Uv.y)).xyz * mask * blink * borderMask.xyz;
	
	float opacity = clamp(distanceToCamera / 50.0, 0.0, 1.0);
	opacity = smoothstep(0.3, 0.4, opacity);
	
	outColor = vec4(color, borderMask.a - opacity);	
}