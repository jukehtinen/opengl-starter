#version 450 core

layout(triangles, equal_spacing, ccw) in;

layout(location = 0) in vec2 inUv[];
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec3 inFragPos[];

layout(binding = 0) uniform sampler2D texHeight;

uniform mat4 vp;
uniform float displacement;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPos;

out gl_PerVertex
{
    vec4 gl_Position;
};

vec2 interpolate2D(vec2 v0, vec2 v1, vec2 v2)
{
    return vec2(gl_TessCoord.x) * v0 + vec2(gl_TessCoord.y) * v1 + vec2(gl_TessCoord.z) * v2;
}

vec3 interpolate3D(vec3 v0, vec3 v1, vec3 v2)
{
    return vec3(gl_TessCoord.x) * v0 + vec3(gl_TessCoord.y) * v1 + vec3(gl_TessCoord.z) * v2;
}

void main()
{
    outUv = interpolate2D(inUv[0], inUv[1], inUv[2]);
    outNormal = normalize(interpolate3D(inNormal[0], inNormal[1], inNormal[2]));
    outFragPos = interpolate3D(inFragPos[0], inFragPos[1], inFragPos[2]);

    outFragPos += vec3(0.0, 1.0, 0.0) * texture(texHeight, outUv.xy).x * displacement;
    gl_Position = vp * vec4(outFragPos, 1.0);
}
