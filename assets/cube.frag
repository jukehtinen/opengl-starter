#version 450 core

layout(binding = 0) uniform sampler2D texSampler;
layout(binding = 1) uniform sampler2D shadowMap;

layout(location = 0) in vec2 Uv;
layout(location = 1) in vec3 Normal;
layout(location = 2) in vec3 FragPos;
layout(location = 3) in vec3 FragPosWs;
layout(location = 4) in vec3 NormalWs;
layout(location = 5) in vec4 FragPosLightSpace;

layout(location = 0) out vec4 outColor;
layout(location = 1) out vec3 outNormal;

uniform vec3 lightDir;

float ShadowCalculation(vec4 fragPosLightSpace, vec3 normal)
{
    vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
    projCoords = projCoords * 0.5 + 0.5;

    float closestDepth = texture(shadowMap, projCoords.xy).r; 
    float currentDepth = projCoords.z;
	float bias = 0.005;
	
	// PCF
    float shadow = 0.0;
    vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
    for(int x = -1; x <= 1; ++x)
    {
        for(int y = -1; y <= 1; ++y)
        {
            float pcfDepth = texture(shadowMap, projCoords.xy + vec2(x, y) * texelSize).r; 
            shadow += currentDepth - bias > pcfDepth ? 0.5 : 1.0;        
        }    
    }
    shadow /= 9.0;

    return shadow;
}  

void main(void)
{
    vec3 norm = normalize(NormalWs);
    float diff = max(dot(norm, normalize(lightDir)), 0.0);
    
    float shadow = ShadowCalculation(FragPosLightSpace, normalize(NormalWs));

    outColor = texture(texSampler, Uv) * clamp(diff, 0.2, 1.0) * shadow;
	outNormal = normalize(Normal);
}