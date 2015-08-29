#version 330 core

uniform int WIDTH;
uniform samplerBuffer tex_velocity;

layout(location = 0) in vec4 iVelocity;
layout(location = 1) in vec2 iPressure;
layout(location = 2) in ivec4 iConnections;

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

out vec4 oColor;

void main() {
	oColor = vec4(iVelocity.xy, 0, 1);

	int n = iConnections[0] + 1;
	gl_Position = ciProjectionMatrix * ciModelView * vec4(mod(n, WIDTH), float(n) / float(WIDTH), 0, 1);
}