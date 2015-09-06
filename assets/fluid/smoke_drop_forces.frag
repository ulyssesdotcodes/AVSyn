#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_velocity;
uniform sampler2D tex_pressure;

uniform vec2 smokeDropPos;
uniform vec2 smokeVel;

uniform float time;
uniform float dt;
uniform float beat;

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

	vec4 vel = texture2D(tex_velocity, pos + offset);
	return vec4(-vel.xy, vel.z,  1);
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy ;

	vec2 sPRel = vec2(smokeDropPos.x, 1.0 - smokeDropPos.y) - pos;

	if(smokeDropPos.x > 0 && smokeDropPos.y > 0 && dot(sPRel, sPRel) < 0.008) {
		float lm = length(smokeVel);

		vec2 smokePrev = smokeDropPos - smokeVel;
		float projection = dot(smokePrev - pos, smokeVel / lm);
		float fp = projection / lm;
		float dist;
		if(projection < 0.0){
			dist = length(pos - smokePrev);
		}
		else if(projection > lm) {
			dist = length(pos - smokeDropPos);
		}
		else {
			dist = sqrt(abs(dot(pos - smokePrev, pos - smokePrev) - projection*projection));
		}

		float pf = 1.0 - clamp(fp, 0.0, 1.0) * 0.6;
		float m = exp(-dist / 0.025);

		m *= pf * pf;

		v += (smokeVel - v - sPRel) * m;

		velocity.z = 1.0 + 0.06 * beat;
	}

	// Initialize T to 1.0
	if(velocity.z <= 0.00001) {
		velocity.z = 1.0;
	}

	v.y += 0.2 * (1.0 - 1 / velocity.z);

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
