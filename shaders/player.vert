#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
	mat4 view;
	mat4 proj;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec2 inTexCoord;

layout(location = 0) out vec2 fragTexCoord;

layout(push_constant) uniform Push {
    vec3 transform;
    float rotation;
} push;

vec3 rotate(vec3 position, float angle) {
	return vec3(
		position.x * cos(angle) - position.y * sin(angle),
		position.x * sin(angle) + position.y * cos(angle),
		position.z
	);
}

void main() {
	vec3 rotatedPosition = rotate(inPosition, radians(push.rotation));
	gl_Position = ubo.proj * ubo.view * vec4(rotatedPosition + push.transform, 1.0);
	fragTexCoord = inTexCoord;
}