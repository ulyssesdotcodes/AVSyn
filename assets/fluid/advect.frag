#version 330 core

uniform vec2 i_resolution;
uniform vec2 i_target_resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_target;
uniform float i_dt;

out vec4 fragColor;

vec4 boundary(vec2 targetPos) {
	vec4 outVel;
	vec2 offset = vec2(0, 0);

	if(targetPos.x <= 1. / i_target_resolution.x) {
		offset.x = 1.1/i_target_resolution.x;
	}
	else if(targetPos.x * i_target_resolution.x >= i_target_resolution.x - 1) {
		offset.x = -1.1/i_target_resolution.x;
	}

	if(targetPos.y <= 1. / i_target_resolution.y) {
		offset.y = 1.1/i_target_resolution.y;
	}
	else if(targetPos.y * i_target_resolution.y >= i_target_resolution.y - 1) {
		offset.y = -1.1/i_target_resolution.y;
	}

	return texture2D(tex_target, targetPos + offset);
}

vec4 inner(vec2 targetPos) {
	vec4 velocity = texture2D(tex_velocity, targetPos);
	vec2 resPos = floor(targetPos * i_target_resolution) + 0.5;
	vec2 tracedPos = resPos - i_dt * velocity.xy * i_target_resolution / i_resolution;
	
	// Calculate the top left corner of the nearest 4 pixels
	vec2 flooredPos = floor(tracedPos - 0.5) + 0.5;

	vec2 t = fract(tracedPos);

	vec4 tex11 = texture2D(tex_target, flooredPos / i_target_resolution); // Top left
	vec4 tex12 = texture2D(tex_target, (flooredPos + vec2(1, 0)) / i_target_resolution.xy); // Top right
	vec4 tex21 = texture2D(tex_target, (flooredPos + vec2(0, 1)) / i_target_resolution.xy); // Bottom left
	vec4 tex22 = texture2D(tex_target, (flooredPos + vec2(1, 1)) / i_target_resolution.xy); // Bottom right

	return mix(mix(tex11, tex12, t.x), mix(tex21, tex22, t.x), t.y);
}

void main() {
	vec2 targetPos = gl_FragCoord.xy / i_target_resolution.xy;
	vec4 outVelocity;
	if(targetPos.x <= 1. / i_target_resolution.x || targetPos.y <= 1. / i_target_resolution.y || targetPos.x >= 1.0 - 1.1 /i_target_resolution.x || targetPos.y >= 1.0 - 1.1 / i_target_resolution.y) {
		outVelocity = boundary(targetPos);
	}
	else {
		outVelocity = inner(targetPos);
	}

	fragColor = outVelocity;
}
