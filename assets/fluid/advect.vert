#version 330 core

uniform int WIDTH;

layout (location = 0) in vec2 iVelocity;
layout (location = 2) in ivec4 iConnections;

uniform samplerBuffer tex_target;
uniform float dt;

out vec4 tf_target;

vec4 boundary() {
	vec4 outVel = vec4(0);

	if(iConnections[0] == -1 && iConnections[2] != -1) {
		outVel = texelFetch(tex_target, iConnections[2]);
	}
	else if(iConnections[1] == -1 && iConnections[3] != -1) {
		outVel = texelFetch(tex_target, iConnections[3]);
	}
	else if(iConnections[2] == -1 && iConnections[0] != -1) {
		outVel = texelFetch(tex_target, iConnections[0]);
	}
	else if(iConnections[3] == -1 && iConnections[1] != -1) {
		outVel = texelFetch(tex_target, iConnections[1]);
	}

	return outVel;
}

vec4 inner() {
	int n = iConnections[0] + 1;
	// We're finding the previous position based on velocity, but we have to convert velocity into 1D first.
	vec2 tracedPos = vec2(mod(n, WIDTH), n / float(WIDTH)) - dt * iVelocity;

	// Calculate the top left corner of the nearest 4 pixels
	vec2 flooredPos = floor(tracedPos - 0.5) + 0.5;
	tracedPos = fract(tracedPos);

	int n11 = int(tracedPos.x) + WIDTH * int(tracedPos.y);


	vec4 tex11 = texelFetch(tex_target, n11); // Top left
	vec4 tex12 = texelFetch(tex_target, n11 + 1); // Top right
	vec4 tex21 = texelFetch(tex_target, n11 + WIDTH); // Bottom left
	vec4 tex22 = texelFetch(tex_target, n11 + 1 + WIDTH); // Bottom right

	return mix(mix(tex11, tex21, tracedPos.y), mix(tex21, tex22, tracedPos.y), tracedPos.x);
}

void main() {
	vec4 outVelocity;
	if(iConnections[0] == -1 || iConnections[1] == -1 || iConnections[2] == -1 || iConnections[3] == -1) {
		outVelocity = boundary();
	}
	else {
		outVelocity = inner();
	}

	tf_target = outVelocity;
}