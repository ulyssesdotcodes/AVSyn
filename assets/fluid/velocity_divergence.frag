#version 330 core

uniform vec2 i_resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_pressure;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec2 offset = vec2(0, 0);

	if(pos.x <= 1. / i_resolution.x) {
		offset.x = 1.1/i_resolution.x;
	}
	else if(pos.x * i_resolution.x >= i_resolution.x - 1) {
		offset.x = -1.1/i_resolution.x;
	}

	if(pos.y <= 1. / i_resolution.y) {
		offset.y = 1.1/i_resolution.y;
	}
	else if(pos.y * i_resolution.y >= i_resolution.y - 1) {
		offset.y = -1.1/i_resolution.y;
	}

	return vec4(0, texture2D(tex_pressure, pos + offset).y, 0, 1);
}

vec4 inner(vec2 pos) {
	vec4 L = texture2D(tex_velocity, pos + vec2(-1, 0) / i_resolution.xy);
	vec4 T = texture2D(tex_velocity, pos + vec2(0, 1) / i_resolution.xy);
	vec4 R = texture2D(tex_velocity, pos + vec2(1, 0) / i_resolution.xy);
	vec4 B = texture2D(tex_velocity, pos + vec2(0, -1) / i_resolution.xy);

	return vec4(((R.x - L.x) + (T.y - B.y)), 0, 0, 1);
}

void main() {
	vec4 outDP;
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;

	if(pos.x <= 1. / i_resolution.x || pos.y <= 1. / i_resolution.y || pos.x >= 1.0 - 1.1 / i_resolution.x || pos.y >= 1.0 - 1.1 / i_resolution.y) {
		outDP = boundary(pos);
	}
	else {
		outDP = inner(pos);
	}

	fragColor = outDP;
}
