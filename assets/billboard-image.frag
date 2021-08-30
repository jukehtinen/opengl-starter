#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 Uv;

layout(location = 0) out vec4 outColor;

uniform float time;

vec2 rotateUV(vec2 uv, float rotation, vec2 mid)
{
    return vec2(
      cos(rotation) * (uv.x - mid.x) + sin(rotation) * (uv.y - mid.y) + mid.x,
      cos(rotation) * (uv.y - mid.y) - sin(rotation) * (uv.x - mid.x) + mid.y
    );
}

void main(void)
{
    outColor = texture(texSampler, rotateUV(Uv, time * distance(Uv, vec2(0.5, 0.5)), vec2(0.5, 0.5))) * vec4(1.0, 0.0, 0.02, 1.0);
	outColor.a *=2; 
}