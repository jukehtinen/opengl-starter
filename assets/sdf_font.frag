#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUv;
layout(location = 1) in vec4 inColor;

out vec4 outColor;

void main() 
{
    float distance = texture2D(texSampler, inUv).a;
    float smoothWidth = fwidth(distance);
    float alpha = smoothstep(0.5 - smoothWidth, 0.5 + smoothWidth, distance);
    vec3 rgb = vec3(alpha);
    
    // todo - hardcoded outline
    float outline = 1.0;
    float outlineWidth = 0.6;
    vec4 outlineColor = vec4(0.0, 0.0, 0.0, 1.0);
	if (outline > 0.0) 
	{
		float w = 1.0 - outlineWidth;
		alpha = smoothstep(w - smoothWidth, w + smoothWidth, distance);
        rgb += mix(vec3(alpha), outlineColor.rgb, alpha);
    }	

    outColor = vec4(rgb * inColor.xyz, alpha * inColor.a);
}