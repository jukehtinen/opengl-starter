#version 450 core

layout(location = 0) in vec3 Pos;
layout(location = 1) in vec2 Uv;
layout(location = 2) in vec3 Normal;

layout(binding = 0) uniform sampler2D texHeight;

uniform mat4 vp;
uniform mat4 model;
uniform float displacement;

layout(location = 0) out vec2 outUv;
layout(location = 1) out vec3 outNormal;
layout(location = 2) out vec3 outFragPos;

out gl_PerVertex
{
    vec4 gl_Position;
};

void main(void)
{
    vec3 pos = Pos;
	pos.y = texture(texHeight, Uv).x * displacement;
	
	mat3 normalMatrix = mat3(transpose(inverse(model)));
	
	float offset = 0.05;
	vec3 neighbour1 = Pos + vec3(offset, 0, 0);
	vec3 neighbour2 = Pos + vec3(0, 0, -offset);
	neighbour1.y = (texture(texHeight, neighbour1.xz).x * displacement);
	neighbour2.y = (texture(texHeight, neighbour2.xz).x * displacement);

	vec3 tangent = neighbour1 - pos;
	vec3 bitangent = neighbour2 - pos;

	outNormal = normalize(cross(tangent, bitangent));
    outUv = Uv;
    outFragPos = vec3(model * vec4(pos, 1.0));

    gl_Position = vp * model * vec4(pos, 1.0);
}