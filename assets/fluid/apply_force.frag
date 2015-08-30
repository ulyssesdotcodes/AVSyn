#version 330 core

uniform vec2 resolution;
uniform vec2 windowRes;
uniform sampler2D tex_velocity;

uniform float time;
uniform float dt;

uniform bool isMouseDown;
uniform vec2 mouse;
uniform vec2 lastMouse;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec4 outVel;
	if(pos.x <= 1. / resolution.x) {
		outVel = texture2D(tex_velocity, vec2(0 + 1.1/resolution.x, pos.y));
	}
	else if(pos.y == 1. / resolution.y) {
		outVel = texture2D(tex_velocity, vec2(pos.x, 0 + 1.1/resolution.y));
	}
	else if(floor(pos.x * resolution.x) == resolution.x - 1) {
		outVel = texture2D(tex_velocity, vec2(1.0 - 1.1/resolution.x, pos.y));
	}
	else if(floor(pos.y * resolution.y) == resolution.y - 1) {
		outVel = texture2D(tex_velocity, vec2(pos.x, 1.0 - 1.1/resolution.x));
	}

	return -outVel;
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy * 0.9;

	if(isMouseDown) {

		vec2 mouseVel = -(lastMouse - mouse) / dt;
		mouseVel.y = -mouseVel.y;

		//v += 0.01 * mouseVel * dt * exp((pow(pos.x - lastMouse.x,  2) + pow(pos.y  - lastMouse.y, 2)) / 0.25);

		float F = max(0.5 - length(vec2(lastMouse.x, 1.0 - lastMouse.y) - pos), 0);
		v += dt * mouseVel * F;

		//float xDist = max(0.1 - abs(pos.x - mouse.x), 0);

		//v += vec2(mouseVel.y * xDist, xDist);
	}

	return vec4(v, 0, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x <= 1. / resolution.x || pos.y <= 1. / resolution.y || floor(pos.x * resolution.x) == resolution.x - 1 || floor(pos.y * resolution.y) == resolution.y - 1) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	fragColor = outVelocity;
}
