#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_pressure;
uniform sampler2D tex_velocity;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec2 offset = vec2(0, 0);

	if(pos.x <= 1. / resolution.x) {
		offset.x = 1.1/resolution.x;
	}
	else if(pos.x >= 1.0 - 1. / resolution.x) {
		offset.x = -1.1/resolution.x;
	}

	if(pos.y <= 1. / resolution.y) {
		offset.y = 1.1/resolution.y;
	}
	else if(pos.y >= 1.0 - 1. / resolution.y) {
		offset.y = -1.1/resolution.y;
		return vec4(texture2D(tex_velocity, pos + offset).xyz, 1);
	}

	return vec4(-texture2D(tex_velocity, pos + offset).xyz, 1);
}

vec4 inner(vec2 pos) {
	float L = texture2D(tex_pressure, pos + vec2(-1, 0) / resolution.xy).y;
	float T = texture2D(tex_pressure, pos + vec2(0, 1) / resolution.xy).y;
	float R = texture2D(tex_pressure, pos + vec2(1, 0) / resolution.xy).y;
	float B = texture2D(tex_pressure, pos + vec2(0, -1) / resolution.xy).y;

	vec4 velocity = texture2D(tex_velocity, pos);
//velocity.xy - 0.5 * vec2(R-L, T-B)
	return vec4(velocity.xy - 0.5 * vec2(R-L, T-B), velocity.z, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x <= 1. / resolution.x || pos.y <= 1. / resolution.y || pos.x >= 1.0 - 1 / resolution.x || pos.y >= 1.0 - 1 / resolution.y) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
