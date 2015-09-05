#version 330 core

uniform vec2 resolution;
uniform vec2 target_resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_target;
uniform sampler2D phi_n_1_hat;
uniform sampler2D phi_n;
uniform sampler2D phi_n_hat;
uniform float dt;
uniform bool boundaryConditions;

out vec4 fragColor;

vec4 boundary(vec2 targetPos) {
	if(!boundaryConditions) {
		return texture2D(tex_target, targetPos);
	}

	vec4 outVel;
	vec2 offset = vec2(0, 0);

	if(targetPos.x <= 1. / target_resolution.x) {
		offset.x = 1.1/target_resolution.x;
	}
	else if(targetPos.x * target_resolution.x >= target_resolution.x - 1) {
		offset.x = -1.1/target_resolution.x;
	}

	if(targetPos.y <= 1. / target_resolution.y) {
		offset.y = 1.1/target_resolution.y;
	}
	else if(targetPos.y * target_resolution.y >= target_resolution.y - 1) {
		offset.y = -1.1/target_resolution.y;
		vec4 vel = texture2D(tex_target, targetPos + offset);
		return vec4(vel.xyz, 1);
	}

	vec4 vel = texture2D(tex_target, targetPos + offset);
	outVel = vec4(-vel.xy, vel.z, 1);

	return outVel;
}

vec4 inner(vec2 targetPos) {
	vec4 velocity = texture2D(tex_velocity, targetPos);
	vec2 resPos = floor(targetPos * target_resolution) + 0.5;
	vec2 tracedPos = resPos - dt * velocity.xy * target_resolution ;
	
	// Calculate the top left corner of the nearest 4 pixels
	vec2 flooredPos = floor(tracedPos + 0.5);

	vec2 t = fract(tracedPos);

	vec4 tex11 = texture2D(phi_n, flooredPos / target_resolution.xy); // Top left
	vec4 tex12 = texture2D(phi_n, (flooredPos + vec2(1, 0)) / target_resolution.xy); // Top right
	vec4 tex21 = texture2D(phi_n, (flooredPos + vec2(0, 1)) / target_resolution.xy); // Bottom left
	vec4 tex22 = texture2D(phi_n, (flooredPos + vec2(1, 1)) / target_resolution.xy); // Bottom right

	vec4 phiMin = min(min(min(tex11, tex12), tex21), tex22);
	vec4 phiMax = max(max(max(tex11, tex12), tex21), tex22);

	vec2 scaledPos = flooredPos / target_resolution;

	vec4 result = texture2D(phi_n_1_hat, scaledPos) + 0.5 * (texture2D(phi_n, scaledPos) - texture2D(phi_n_hat, scaledPos));

	result = max(min(result, phiMax), phiMin);

	return result;
}

void main() {
	vec2 targetPos = gl_FragCoord.xy / target_resolution.xy;
	vec4 outVelocity;
	if(targetPos.x <= 1. / target_resolution.x || targetPos.y <= 1. / target_resolution.y || targetPos.x >= 1.0 - 1.1 /target_resolution.x || targetPos.y >= 1.0 - 1.1 / target_resolution.y) {
		outVelocity = boundary(targetPos);
	}
	else {
		outVelocity = inner(targetPos);
	}

	fragColor = outVelocity;
}
