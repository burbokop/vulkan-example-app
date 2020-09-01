#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(binding = 2, set = 2) uniform sampler2D texSampler;

void main() {
	outColor = (texture(texSampler, fragTexCoord) * 0.8 + vec4(fragTexCoord, 0.0, 1.0) * 0.2);
	//outColor = vec4(fragColor, 1.0);
	//outColor = texture(texSampler, fragTexCoord);
	//outColor.w = 0.3;
	//outColor = vec4(fragTexCoord, 0.0, 1.0);
}


