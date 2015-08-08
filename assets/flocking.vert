#version 330 core

uniform mat4 ciModelViewProjection;

layout(location = 0) in vec3 position;
layout(location = 2) in vec3 color;

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

//out float pointSize;
out vec3 oColor;
out float pointSize;

void main() {
	vec4 mvPosition = ciModelView * vec4(position.xyz, 1.0);
	gl_PointSize = 500.0 / length(mvPosition.xyz);
	oColor = color;
	gl_Position = ciProjectionMatrix * mvPosition;
}