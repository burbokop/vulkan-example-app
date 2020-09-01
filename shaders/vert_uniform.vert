#version 450
#extension GL_ARB_separate_shader_objects : enable

layout(binding = 0, set = 0) uniform UniformBufferObject {
	vec2 offset;
	float time;
} ubo;

layout(binding = 1, set = 1) uniform OM { 
	mat4 model;
} object;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

float random (vec3 st) {
    return fract(sin(dot(st.xyz, vec3(12.9898,78.233, 3.1234))) * 43758.5453123);
}

void main() {
    	//gl_Position = ubo.proj * ubo.view * ubo.model * object.model * vec4(inPosition, 0.0, 1.0);
	gl_Position = object.model * vec4(inPosition, 1.0) + vec4(ubo.offset, 0.0, 0.0);



    	fragColor = inColor * mod(random(inPosition), 1.);

	//fragColor = vec3(inTexCoord, 0);

	fragTexCoord = inTexCoord;
}
