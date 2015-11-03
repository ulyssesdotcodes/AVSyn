#version 330 core

uniform vec2 i_resolution;
uniform sampler2D tex_pressure;
uniform sampler2D tex_velocity;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec2 offset = vec2(0, 0);

	if(pos.x <= 1. / i_resolution.x) {
		offset.x = 1.1/i_resolution.x;
	}
	else if(pos.x >= 1.0 - 1. / i_resolution.x) {
		offset.x = -1.1/i_resolution.x;
	}

	if(pos.y <= 1. / i_resolution.y) {
		offset.y = 1.1/i_resolution.y;
	}
	else if(pos.y >= 1.0 - 1. / i_resolution.y) {
		offset.y = -1.1/i_resolution.y;
	}

	vec4 vel = texture2D(tex_velocity, pos + offset);
	return vec4(-vel.xy, vel.z, 1);
}

vec4 inner(vec2 pos) {
	float L = texture2D(tex_pressure, pos + vec2(-1, 0) / i_resolution.xy).y;
	float T = texture2D(tex_pressure, pos + vec2(0, 1) / i_resolution.xy).y;
	float R = texture2D(tex_pressure, pos + vec2(1, 0) / i_resolution.xy).y;
	float B = texture2D(tex_pressure, pos + vec2(0, -1) / i_resolution.xy).y;

	vec4 velocity = texture2D(tex_velocity, pos);
	return vec4(velocity.xy - 0.5 * vec2(R-L, T-B), velocity.z, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	if(pos.x <= 1. / i_resolution.x || pos.y <= 1. / i_resolution.y || pos.x >= 1.0 - 1.0 / i_resolution.x || pos.y >= 1.0 - 1.0 / i_resolution.y) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
