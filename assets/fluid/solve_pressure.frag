#version 330 core

uniform vec2 i_resolution;
uniform sampler2D tex_pressure;

out vec4 fragColor;

vec2 inner(vec2 pos) {
	float L = texture2D(tex_pressure, pos + vec2(-1, 0) / i_resolution.xy).y;
	float T = texture2D(tex_pressure, pos + vec2(0, 1) / i_resolution.xy).y;
	float R = texture2D(tex_pressure, pos + vec2(1, 0) / i_resolution.xy).y;
	float B = texture2D(tex_pressure, pos + vec2(0, -1) / i_resolution.xy).y;

	float bC = texture2D(tex_pressure, pos).x;

	return vec2(bC, (L + R + B + T - bC) * .25);
}

vec2 boundary(vec2 pos) {
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

	return texture2D(tex_pressure, pos + offset).xy;
}

void main() {
	vec2 outDP;
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	if(pos.x <= 1. / i_resolution.x || pos.y <= 1. / i_resolution.y || pos.x >= 1.0 - 1.1 / i_resolution.x || pos.y >= 1.0 - 1.1 / i_resolution.y) {
		outDP = boundary(pos);
	}
	else {
		outDP = inner(pos);
	}

	fragColor = vec4(outDP, 0, 1);
}
