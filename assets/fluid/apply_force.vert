#version 330 core

uniform int WIDTH;

layout (location = 0) in vec4 iVelocity;
layout (location = 2) in ivec4 iConnections;

uniform float dt;

out vec4 tf_velocity;

vec4 boundary() {
	return iVelocity;
}

vec4 inner() {
	float x = float(mod(iConnections[0] + 1, WIDTH)) / float(WIDTH);
	float F = 0.5 - abs(0.5 - x);
	return iVelocity + dt * vec2(0, 1) * F;
}

void main() {
	vec4 outVelocity;
	if(iConnections[0] == -1 || iConnections[1] == -1 || iConnections[2] == -1 || iConnections[3] == -1) {
		outVelocity = boundary();
	}
	else {
		outVelocity = inner();
	}

	tf_velocity = outVelocity;
}
