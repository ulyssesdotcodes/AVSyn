#version 330 core

layout (location = 0) in vec2 iDivergePressure;
layout (location = 1) in ivec4 iConnections;

uniform samplerBuffer tex_velocity;

out vec2 tf_pressure;

vec2 boundary() {
	return iDivergePressure;
}

vec2 inner() {
	vec2 L = texelFetch(tex_velocity, iConnections[0]).xy;
	vec2 T = texelFetch(tex_velocity, iConnections[1]).xy;
	vec2 R = texelFetch(tex_velocity, iConnections[2]).xy;
	vec2 B = texelFetch(tex_velocity, iConnections[3]).xy;

	return 0.5 * vec2((R.x - L.x) + (T.y - B.y), 0);
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