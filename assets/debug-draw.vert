#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec4 Color;

uniform mat4 vp;

layout(location = 0) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    outColor = Color;
    gl_Position = vp * vec4(Pos, 1.0);
}