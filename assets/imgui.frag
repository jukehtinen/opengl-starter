#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUv;
layout(location = 1) in vec4 inColor;

out vec4 outColor;

void main()
{
    outColor = inColor * texture(texSampler, inUv);
}
