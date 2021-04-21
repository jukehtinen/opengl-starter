#version 450 core

layout(vertices = 3) out;

layout(location = 0) in vec2 inUv[];
layout(location = 1) in vec3 inNormal[];
layout(location = 2) in vec3 inFragPos[];

uniform vec3 eye;

layout(location = 0) out vec2 outUv[];
layout(location = 1) out vec3 outNormal[];
layout(location = 2) out vec3 outFragPos[];

float GetTessLevel(float Distance0, float Distance1)
{
    float AvgDistance = (Distance0 + Distance1) / 2.0;

    if (AvgDistance <= 10.0)
    {
        return 10.0;
    }
    else if (AvgDistance <= 20.0)
    {
        return 4.0;
    }
    else
    {
        return 1.0;
    }
}

void main()
{
    outUv[gl_InvocationID] = inUv[gl_InvocationID];
    outNormal[gl_InvocationID] = inNormal[gl_InvocationID];
    outFragPos[gl_InvocationID] = inFragPos[gl_InvocationID];

    // Calculate the distance from the camera to the three control points
    float EyeToVertexDistance0 = distance(eye, outFragPos[0]);
    float EyeToVertexDistance1 = distance(eye, outFragPos[1]);
    float EyeToVertexDistance2 = distance(eye, outFragPos[2]);

    // Calculate the tessellation levels
    gl_TessLevelOuter[0] = GetTessLevel(EyeToVertexDistance1, EyeToVertexDistance2);
    gl_TessLevelOuter[1] = GetTessLevel(EyeToVertexDistance2, EyeToVertexDistance0);
    gl_TessLevelOuter[2] = GetTessLevel(EyeToVertexDistance0, EyeToVertexDistance1);
    gl_TessLevelInner[0] = gl_TessLevelOuter[2];
}