#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_pressure;
uniform sampler2D tex_velocity;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	return texture2D(tex_velocity, pos);
}

vec4 inner(vec2 pos) {
	float L = texture2D(tex_pressure, pos + vec2(-1, 0) / resolution.xy).y;
	float T = texture2D(tex_pressure, pos + vec2(0, 1) / resolution.xy).y;
	float R = texture2D(tex_pressure, pos + vec2(1, 0) / resolution.xy).y;
	float B = texture2D(tex_pressure, pos + vec2(0, -1) / resolution.xy).y;

	vec4 velocity = texture2D(tex_velocity, pos);

	return vec4(velocity.xy - 0.5 * vec2(R-L, T-B), 0, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x == 0 || pos.y == 0 || floor(pos.x * resolution.x) == resolution.x - 1 || floor(pos.y * resolution.y) == resolution.y - 1) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
