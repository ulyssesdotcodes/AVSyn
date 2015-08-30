#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_pressure;

out vec2 tf_pressure;

vec2 inner(vec2 pos) {
	float L = texture2D(tex_pressure, pos + vec2(-1, 0) / resolution.xy).y;
	float T = texture2D(tex_pressure, pos + vec2(0, 1) / resolution.xy).y;
	float R = texture2D(tex_pressure, pos + vec2(1, 0) / resolution.xy).y;
	float B = texture2D(tex_pressure, pos + vec2(0, -1) / resolution.xy).y;

	float bC = texture2D(tex_pressure, pos).x;

	return vec2(bC, (L + R + B + T - bC) * .25);
}

vec2 boundary(vec2 pos) {
	vec2 outVel = vec2(0);

	if(pos.x == 0) {
		outVel = texture2D(tex_pressure, vec2(0 + 1./resolution.x, pos.y)).xy;
	}
	else if(pos.y == 0) {
		outVel = texture2D(tex_pressure, vec2(pos.x, 0 + 1./resolution.y)).xy;
	}
	else if(floor(pos.x * resolution.x) == resolution.x - 1) {
		outVel = texture2D(tex_pressure, vec2(1.0 - 1./resolution.x, pos.y)).xy;
	}
	else if(floor(pos.y * resolution.y) == resolution.y - 1) {
		outVel = texture2D(tex_pressure, vec2(pos.x, 1.0 - 1./resolution.y)).xy;
	}

	return outVel;
}

void main() {
	vec2 outDP;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x == 0 || pos.y == 0 || floor(pos.x * resolution.x) == resolution.x - 1 || floor(pos.y * resolution.y) == resolution.y - 1) {
		outDP = boundary(pos);
	}
	else {
		outDP = inner(pos);
	}

	tf_pressure = outDP;
}
