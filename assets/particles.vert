#version 450 core

const vec2 uvs[6] = vec2[] (
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0)
);

struct Particle
{
	vec4 Position;
	vec4 Velocity;
	vec4 Color;	
	vec4 Param; //life, lifemax, width, height
	vec4 Param2; //rotspeed, rot
};

layout(std430, binding = 0) buffer particleBuffer
{
    Particle particles[];
};

uniform mat4 model;
uniform mat4 v;
uniform mat4 p;

layout (location = 0) out vec2 outUv;
layout (location = 1) out float outLife;
layout (location = 2) out vec4 outColor;

out gl_PerVertex
{
	vec4 gl_Position;
};

vec3 rotateAxis(vec3 n, float a, vec3 v)
{
	return normalize(v * cos(a) + (dot(v, n) * n * (1-cos(a))) + (cross(n, v) * sin(a)));
}

void main() 
{
	vec2 uv = uvs[gl_VertexID];
	vec3 pos = (v * model * vec4(particles[gl_InstanceID].Position.xyz, 1.0)).xyz;
	
	pos += particles[gl_InstanceID].Param.z * (0.5 - uv.x) * rotateAxis(vec3(0.0, 0.0, 1.0), particles[gl_InstanceID].Param2.y, vec3(1.0, 0.0, 0.0));
    pos += particles[gl_InstanceID].Param.w * (0.5 - uv.y) * rotateAxis(vec3(0.0, 0.0, 1.0), particles[gl_InstanceID].Param2.y, vec3(0.0, 1.0, 0.0));
		
	outUv = uv;	
	outLife = clamp(particles[gl_InstanceID].Param.x / particles[gl_InstanceID].Param.y, 0.0, 1.0);
	outColor = particles[gl_InstanceID].Color;
	
	gl_Position = p * vec4(pos, 1.0);	
}
