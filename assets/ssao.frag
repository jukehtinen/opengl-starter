#version 450 core

layout(binding = 0) uniform sampler2D texNormal;
layout(binding = 1) uniform sampler2D texDepth;
layout(binding = 2) uniform sampler2D texNoise;

layout(location = 0) in vec2 Uv;

uniform float radius = 0.5f;
uniform float strength = 1.0f;
uniform float bias = 0.025f;
uniform float area = 0.0075;
uniform vec3 sampleSphere[256];
uniform int kernelSize = 128;

uniform mat4 projection;
uniform vec2 winSize;

out vec4 outColor;
     
vec3 getFragPos(vec2 coords)
{
	float depth = texture(texDepth, coords).x;
	vec4 p = inverse(projection) * (vec4(coords, depth, 1.0) * 2.0 - 1.0);
	return p.xyz / p.w;
}

void main() 
{
	int kernelSize = 64;
	    	
	vec3 fragPos = getFragPos(Uv);
		
    vec3 normal = normalize(texture(texNormal, Uv).rgb);
		
	vec2 noiseScale = winSize / 4.0;
	
    vec3 randomVec = normalize(texture(texNoise, Uv * noiseScale).xyz);
	
    // create TBN change-of-basis matrix: from tangent-space to view-space
	vec3 tangent = normalize(randomVec - normal * dot(randomVec, normal));
    vec3 bitangent = cross(normal, tangent);
    mat3 TBN = mat3(tangent, bitangent, normal);
    
	// iterate over the sample kernel and calculate occlusion factor
    float occlusion = 0.0;
    for(int i = 0; i < kernelSize; ++i)
    {
        // get sample position
        vec3 samplePos = TBN * sampleSphere[i]; // from tangent to view-space
        samplePos = fragPos + samplePos * radius; 
        
        // project sample position (to sample texture) (to get position on screen/texture)
        vec4 offset = vec4(samplePos, 1.0);
        offset = projection * offset; // from view to clip-space
        offset.xyz /= offset.w; // perspective divide
        offset.xyz = offset.xyz * 0.5 + 0.5; // transform to range 0.0 - 1.0
                
		float sampleDepth = getFragPos(offset.xy).z;
        
        // range check & accumulate
        float rangeCheck = smoothstep(0.0, 1.0, radius / abs(fragPos.z - sampleDepth));
        occlusion += (sampleDepth >= samplePos.z + bias ? 1.0 : 0.0) * rangeCheck;
    }
    occlusion = 1.0 - (occlusion / kernelSize);
    
	occlusion = pow(occlusion, strength);	
	
	outColor = vec4(occlusion, occlusion, occlusion, 1.0);
}
