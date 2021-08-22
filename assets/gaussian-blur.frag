#version 450 core

layout(binding = 0) uniform sampler2D texSampler;

layout(location = 0) in vec2 inUv;

uniform vec2 winSize;
uniform vec2 direction;

out vec4 outColor;

void main()
{
	// https://github.com/Jam3/glsl-fast-gaussian-blur
	vec4 color = vec4(0.0);
		
	vec2 off1 = vec2(1.411764705882353) * direction;
	vec2 off2 = vec2(3.2941176470588234) * direction;
	vec2 off3 = vec2(5.176470588235294) * direction;
	color += texture2D(texSampler, inUv) * 0.1964825501511404;
	color += texture2D(texSampler, inUv + (off1 / winSize)) * 0.2969069646728344;
	color += texture2D(texSampler, inUv - (off1 / winSize)) * 0.2969069646728344;
	color += texture2D(texSampler, inUv + (off2 / winSize)) * 0.09447039785044732;
	color += texture2D(texSampler, inUv - (off2 / winSize)) * 0.09447039785044732;
	color += texture2D(texSampler, inUv + (off3 / winSize)) * 0.010381362401148057;
	color += texture2D(texSampler, inUv - (off3 / winSize)) * 0.010381362401148057;
  
	outColor = color;
}
