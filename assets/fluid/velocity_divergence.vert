#version 330 core

layout (location = 1) in vec2 iDivergePressure;
layout (location = 2) in ivec4 iConnections;

uniform samplerBuffer tex_velocity;

out vec2 tf_pressure;

vec2 boundary() {
	vec2 outVel = vec2(0);

	if(iConnections[0] == -1 && iConnections[2] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[2]).xy;
	}
	else if(iConnections[1] == -1 && iConnections[3] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[3]).xy;
	}
	else if(iConnections[2] == -1 && iConnections[0] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[0]).xy;
	}
	else if(iConnections[3] == -1 && iConnections[1] != -1) {
		outVel = texelFetch(tex_velocity, iConnections[1]).xy;
	}

	return outVel;
}

vec2 inner() {
	vec2 L = texelFetch(tex_velocity, iConnections[0]).xy;
	vec2 T = texelFetch(tex_velocity, iConnections[1]).xy;
	vec2 R = texelFetch(tex_velocity, iConnections[2]).xy;
	vec2 B = texelFetch(tex_velocity, iConnections[3]).xy;

	return vec2((R.x - L.x) + (T.y - B.y), 0);
}

void main() {
	vec2 outDP;
	if(iConnections[0] == -1 || iConnections[1] == -1 || iConnections[2] == -1 || iConnections[3] == -1) {
		outDP = boundary();
	}
	else {
		outDP = inner();
	}

	tf_pressure = outDP;
}