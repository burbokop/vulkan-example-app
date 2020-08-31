#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0) uniform UniformBufferObject {
	vec2 offset;
} ubo;

layout(binding = 1, set = 1) uniform OM { 
	mat4 model;
} object;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec3 fragColor;

void main() {
    	//gl_Position = ubo.proj * ubo.view * ubo.model * object.model * vec4(inPosition, 0.0, 1.0);
	gl_Position = object.model * vec4(inPosition, 0.0, 1.0) + vec4(ubo.offset, 0.0, 0.0);
    	fragColor = inColor;
}
