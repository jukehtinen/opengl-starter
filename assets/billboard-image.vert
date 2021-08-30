#version 450 core

const vec2 uvs[6] = vec2[] 
(
	vec2(0.0, 0.0),
	vec2(1.0, 0.0),
	vec2(0.0, 1.0),
	vec2(1.0, 0.0),
	vec2(1.0, 1.0),
	vec2(0.0, 1.0)
);

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

layout (location = 0) out vec2 outUv;

out gl_PerVertex
{
	vec4 gl_Position;
};

void main() 
{
	vec2 uv = uvs[gl_VertexID];

	float sizex = 2.0;
	float sizey = 2.0;

	vec3 rightWs = vec3(view[0][0], view[1][0], view[2][0]);
	vec3 upWs = vec3(view[0][1], view[1][1], view[2][1]); // vec3(0.0, 1.0, 0.0);
	
	vec3 pos = vec3(0.0f, 0.0f, 0.0f);
	pos += sizex * (0.5 - uv.x) * rightWs;
    pos += sizey * (0.5 - uv.y) * upWs;
		
	outUv = vec2(1 - uv.x, uv.y);	
	gl_Position = projection * view * model * vec4(pos, 1.0);	
}
