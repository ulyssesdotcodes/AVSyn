#version 330 core

uniform int WIDTH;
uniform int HEIGHT;

layout (location = 0) in vec4 iVelocity;
layout (location = 2) in ivec4 iConnections;

uniform samplerBuffer tex_velocity;

uniform float time;
uniform float dt;

uniform bool isMouseDown;
uniform ivec2 mouse;
uniform ivec2 lastMouse;

out vec4 tf_velocity;

vec4 boundary() {
	vec4 outVel = vec4(0);

	if(iConnections[0] == -1 && iConnections[2] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[2]);
	}
	else if(iConnections[1] == -1 && iConnections[3] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[3]);
	}
	else if(iConnections[2] == -1 && iConnections[0] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[0]);
	}
	else if(iConnections[3] == -1 && iConnections[1] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[1]);
	}

	return -outVel;
}

vec4 inner() {
	vec2 v = iVelocity.xy * (1.0 - 1.0 * dt);

	if(isMouseDown) {
		vec2 mouseVel = -(lastMouse - mouse) / dt;

		float x = mod(iConnections[0] + 1, WIDTH);
		float y = floor((iConnections[0] + 1) / WIDTH);
		v += mouseVel * dt * exp((pow((x - lastMouse.x) / WIDTH,  2) + pow((y - lastMouse.y) / HEIGHT, 2)) / 10);

  //      float x = mod(iConnections[0] + 1, WIDTH);
		//int y = (iConnections[0] + 1) / WIDTH;

		//float F = max(100 - length(mouse - vec2(x, y)), 0);
		//v += dt * mouseVel * F;
	}

	return vec4(v, 0, 1);
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
