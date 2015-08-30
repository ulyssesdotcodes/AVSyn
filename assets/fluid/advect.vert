#version 330 core

uniform int WIDTH;
uniform int HEIGHT;

layout (location = 0) in vec4 iVelocity;
layout (location = 2) in ivec4 iConnections;

uniform samplerBuffer tex_target;
uniform float dt;

out vec4 tf_target;

vec4 boundary() {
	return iVelocity;
}

vec4 inner() {
	int n = iConnections[0] + 1;
	vec4 outVel;
	if(iVelocity.x == 0 && iVelocity.y == 0) {
		outVel = texelFetch(tex_target, iConnections[0] + 1); // Top left
	}
	else {
		// We're finding the previous position based on velocity, but we have to convert velocity into 1D first.
		vec2 tracedPos = vec2(mod(n, WIDTH), n / WIDTH) - dt * iVelocity.xy;

		// Calculate the top left corner of the nearest 4 pixels
		vec2 flooredPos = floor(tracedPos - 0.5) + 0.5;
		tracedPos = fract(tracedPos);

		int x = max(min(int(flooredPos.x), WIDTH), 0);
		int y = max(min(int(flooredPos.y), HEIGHT - 1), 0);
		int n11 = x + y * WIDTH;
		int n12 = min(x + 1, WIDTH);
		int n21 = x + min(y + 1, HEIGHT - 1) * WIDTH;
		int n22 = min(x + 1, WIDTH) + min(y + 1, HEIGHT - 1) * WIDTH;

		vec4 tex11 = texelFetch(tex_target, n11); // Top left
		vec4 tex12 = texelFetch(tex_target, n12); // Top right
		vec4 tex21 = texelFetch(tex_target, n21); // Bottom left
		vec4 tex22 = texelFetch(tex_target, n22); // Bottom right

		outVel -= mix(mix(tex11, tex21, tracedPos.y), mix(tex21, tex22, tracedPos.y), tracedPos.x);
	}
	return outVel;
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