#version 330 core

layout (location = 0) in vec2 iDivergePressure;
layout (location = 1) in ivec4 iConnections;

uniform samplerBuffer tex_pressure;

out vec2 tf_pressure;

vec2 inner() {
	float L = texelFetch(tex_pressure, iConnections[0]).y;
	float T = texelFetch(tex_pressure, iConnections[1]).y;
	float R = texelFetch(tex_pressure, iConnections[2]).y;
	float B = texelFetch(tex_pressure, iConnections[3]).y;

	float bC = iDivergePressure.x;

	return vec2(bC, (L + R + B + T + bC) * .25);
}

vec2 boundary() {
	vec2 outVel = vec2(0);

	if(iConnections[0] == -1 && iConnections[2] != -1) {
		outVel = texelFetch(tex_pressure, iConnections[2]).xy;
	}
	else if(iConnections[1] == -1 && iConnections[3] != -1) {
		outVel = texelFetch(tex_pressure, iConnections[3]).xy;
	}
	else if(iConnections[2] == -1 && iConnections[0] != -1) {
		outVel = texelFetch(tex_pressure, iConnections[0]).xy;
	}
	else if(iConnections[3] == -1 && iConnections[1] != -1) {
		outVel = texelFetch(tex_pressure, iConnections[1]).xy;
	}

	return outVel;
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