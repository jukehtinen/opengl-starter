#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Uv;
layout(location = 2) in vec3 Normal;

uniform mat4 vp;
uniform mat4 model;
uniform mat4 view;
uniform mat4 lightSpaceMatrix;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPos;
layout(location = 3) out vec3 outFragPosWs;
layout(location = 4) out vec3 outNormalWs;
layout(location = 5) out vec4 outFragPosLightSpace;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    outUv = Uv;
    outNormal = transpose(inverse(mat3(view * model))) * Normal;
    outNormalWs = transpose(inverse(mat3(model))) * Normal;
	vec4 viewPos = view * model * vec4(Pos, 1.0);
    outFragPos = viewPos.xyz;
    outFragPosWs = (model * vec4(Pos, 1.0)).xyz;
    outFragPosLightSpace = lightSpaceMatrix * vec4(outFragPosWs, 1.0);

    gl_Position = vp * model * vec4(Pos, 1.0);
}