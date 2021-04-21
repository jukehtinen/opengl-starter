#version 450 core

layout(binding = 0) uniform sampler2D texHeight;
layout(binding = 1) uniform sampler2D texColor;
layout(binding = 2) uniform sampler2D texColor2;

uniform float white;

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;

out vec4 outColor;

void main(void)
{
    vec3 lightPos = vec3(100.0, 100.0, 0.0);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    if (white > 0.0)
        outColor = vec4(1.0, 1.0, 1.0, 1.0);
    else
        outColor = mix(texture(texColor, Uv), texture(texColor2, Uv), texture(texHeight, Uv).x) * clamp(diff, 0.2, 1.0);
}