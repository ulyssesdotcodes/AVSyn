#version 330 core

layout (location = 0) in vec4 iPosition;	// POSITION_INDEX
layout (location = 1) in vec3 iVelocity;			// VELOCITY_INDEX

uniform samplerBuffer tex_position;

out vec4 oPosition;
out vec3 oVelocity;

void main() {
	oPosition = vec4(iPosition.xyz + iVelocity, 1.0);
	oVelocity = iVelocity;
}