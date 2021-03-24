#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;

out vec4 outColor;

void main(void)
{
    vec3 lightPos = vec3(5.0, 5.0, 5.0);

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    outColor = texture(texSampler, Uv) * clamp(diff, 0.2, 1.0);
}