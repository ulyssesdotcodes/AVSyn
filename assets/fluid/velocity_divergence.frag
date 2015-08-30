#version 330 core

uniform vec2 resolution;

uniform sampler2D tex_velocity;
uniform sampler2D tex_pressure;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	return texture2D(tex_pressure, pos);
}

vec4 inner(vec2 pos) {
	vec4 L = texture2D(tex_velocity, pos + vec2(-1, 0) / resolution.xy);
	vec4 T = texture2D(tex_velocity, pos + vec2(0, 1) / resolution.xy);
	vec4 R = texture2D(tex_velocity, pos + vec2(1, 0) / resolution.xy);
	vec4 B = texture2D(tex_velocity, pos + vec2(0, -1) / resolution.xy);

	return vec4(0.5 * vec2((R.x - L.x) + (T.y - B.y), 0), 0, 1);
}

void main() {
	vec4 outDP;
	vec2 pos = gl_FragCoord.xy / resolution.xy;

	if(pos.x == 0 || pos.y == 0 || floor(pos.x * resolution.x) == resolution.x - 1 || floor(pos.y * resolution.y) == resolution.y - 1) {
		outDP = boundary(pos);
	}
	else {
		outDP = inner(pos);
	}

	fragColor = outDP;
}
