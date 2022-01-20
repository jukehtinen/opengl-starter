#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Uv;
layout(location = 2) in vec3 Normal;

uniform mat4 vp;
uniform mat4 view;
uniform mat4 m;
uniform float time;
uniform float strength = 0.03;
uniform float timescale = 0.085;
uniform float positionscale = 0.2;

layout(std430, binding = 0) buffer modelMatrices
{
    mat4 model[];
};

layout(binding = 2) uniform sampler2D texNoise;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    outUv = Uv;
    outNormal = transpose(inverse(mat3(view * model[gl_InstanceID]))) * Normal;    

    vec3 fragPosWs = (model[gl_InstanceID] * vec4(Pos, 1.0)).xyz;

    vec2 noiseUv = (fragPosWs.xz * positionscale) + (time * timescale);
    vec4 noise = texture(texNoise, noiseUv);

    float effectMask = smoothstep(0.3, 0.4, 1 - Uv.y); 

    vec2 remapped = vec2(noise.x * 2.0f - 1.0f, noise.y * 2.0f - 1.0f);
    vec3 pos = vec3(Pos.x + remapped.x * effectMask * strength, Pos.y, Pos.z + remapped.y * effectMask * strength);

    outColor = noise;

    gl_Position = vp * m * model[gl_InstanceID] * vec4(pos, 1.0);
}