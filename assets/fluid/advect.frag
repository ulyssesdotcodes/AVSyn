#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_target;
uniform float dt;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	return texture2D(tex_target, pos);;
}

vec4 inner(vec2 pos) {
	vec4 outVel;
	vec4 velocity = texture2D(tex_velocity, pos);
	if(velocity.x == 0 && velocity.y == 0) {
		outVel = texture2D(tex_target, pos);
	}
	else {
		vec2 tracedPos = pos - dt * velocity.xy;

		tracedPos = tracedPos * resolution;

		// Calculate the top left corner of the nearest 4 pixels
		vec2 flooredPos = floor(tracedPos - 0.5) + 0.5;
		tracedPos = fract(tracedPos);

		vec2 t = fract(tracedPos);

		vec4 tex11 = texture2D(tex_target, flooredPos / resolution.xy); // Top left
		vec4 tex12 = texture2D(tex_target, (flooredPos + vec2(1, 0)) / resolution.xy); // Top right
		vec4 tex21 = texture2D(tex_target, (flooredPos + vec2(0, 1)) / resolution.xy); // Bottom left
		vec4 tex22 = texture2D(tex_target, (flooredPos + vec2(1, 1)) / resolution.xy); // Bottom right
		outVel = mix(mix(tex11, tex21, t.y), mix(tex21, tex22, t.y), t.x);
	}

	return outVel;
}

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec4 outVelocity;
	if(pos.x <= 1. / resolution.x || pos.y <= 1. / resolution.y || floor(pos.x * resolution.x) == resolution.x - 1 || floor(pos.y * resolution.y) == resolution.y - 1) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = vec4(outVelocity.xyz, 1);
}
