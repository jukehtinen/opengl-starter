#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUv;

uniform vec2 winSize;

out vec4 outColor;

void main() 
{
    vec2 texelSize = 1.0 / winSize;
    float result = 0.0;

    for (int x = -2; x < 2; ++x) 
    {
        for (int y = -2; y < 2; ++y) 
        {
            vec2 offset = vec2(float(x), float(y)) * texelSize;
            result += texture(texSampler, inUv + offset).r;
        }
    }
    outColor = vec4(vec3(result / (4.0 * 4.0)), 1.0);
}  