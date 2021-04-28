#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Uv;
layout(location = 2) in vec3 Normal;

uniform mat4 vp;
uniform mat4 model;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec4 outFragPos;
layout(location = 3) out vec3 outFragPosWs;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    outUv = Uv;
    outNormal = mat3(transpose(inverse(model))) * Normal;
    outFragPosWs = vec3(model * vec4(Pos, 1.0));

    gl_Position = vp * model * vec4(Pos, 1.0);
	outFragPos = gl_Position;
}