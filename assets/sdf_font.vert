#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Uv;
layout(location = 2) in vec4 Color;

uniform mat4 mvp;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec4 outColor;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main() 
{
    outUv = Uv;
    outColor = Color;
    gl_Position = mvp * vec4(Pos, 1.0);
}