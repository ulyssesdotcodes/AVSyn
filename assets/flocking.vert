#version 330 core

uniform mat4 ciModelViewProjection;

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 velocity;
layout(location = 2) in vec3 color;

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

//out float pointSize;
out vec3 oColor;
out float pointSize;
out vec2 oVelocity;

void main() {
	oColor = color;

	vec4 mvPosition = ciModelView * vec4(position.xyz, 1.0);

	float velocityLength = length(velocity);
	vec3 normVel = velocity / velocityLength;

	// PointSize is relative to the model view position
	pointSize = 100.0 * velocityLength * velocityLength / length(mvPosition.xyz);
	gl_PointSize = pointSize * (2.0 + normVel.z);


	oVelocity = (ciProjectionMatrix * vec4(velocity, 0.0)).xy;
	gl_Position = ciProjectionMatrix * mvPosition;
}