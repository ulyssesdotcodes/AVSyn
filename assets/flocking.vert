#version 330 core

uniform mat4 ciModelViewProjection;

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 velocity;

//out float pointSize;
out float hue;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	//color = vec3(1.0, 1.0, 1.0);
	vec4 mvPosition = vec4(position, 1.0);
	//position = mvPosition;
	//pointSize = 500.0 / length(mvPosition.xyz);
	//vColor = vec3(1.0);
	hue = position.x;
	gl_Position = ciModelViewProjection * mvPosition;
}