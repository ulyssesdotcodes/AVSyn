#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_pressure;

uniform vec2 smokeDropPos;

uniform float time;
uniform float dt;

out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec4 boundary(vec2 pos) {
	vec2 offset = vec2(0, 0);

	if(pos.x <= 1. / resolution.x) {
		offset.x = 1/resolution.x;
	}
	else if(pos.x * resolution.x >= resolution.x - 1) {
		offset.x = -1/resolution.x;
	}

	if(pos.y <= 1. / resolution.y) {
		offset.y = 1/resolution.y;
	}
	else if(pos.y * resolution.y >= resolution.y - 1) {
		offset.y = -1/resolution.y;
	}

	return vec4(-texture2D(tex_velocity, pos + offset).xyz, 1);
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy * (1.0 - 1.0 * dt);

	vec2 dist = vec2(smokeDropPos.x, 1.0 - smokeDropPos.y) - pos;

	if(smokeDropPos.x > 0 && smokeDropPos.y > 0 && dot(dist, dist) < 0.0001) {
		velocity.x = 0.2 * (rand(vec2(time, pos.x * pos.y)) - 0.5);
		velocity.y = 0.2 * (rand(vec2(time, pos.x * pos.y)) - 0.5);
		velocity.z = 0.98;
	}

	// Initialize T to 1.0
	if(velocity.z <= 0.00001) {
		velocity.z = 1.0;
	}
	else if(velocity.z < 1.0) {
		float zDif = 1.0 - velocity.z;
		velocity.z = velocity.z + min(zDif * dt * 0.5, zDif);
	}


	float P = texture2D(tex_pressure, pos).y;
	v += -0.0787 * (1.0 - 1 / max(velocity.z, 0.0001)) * vec2(0, 1);

	return vec4(v, velocity.z, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x <= 1. / resolution.x || pos.y <= 1. / resolution.y || pos.x >= 1.0 - 1.1 / resolution.x || pos.y >= 1.0 - 1.1 / resolution.y) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
