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

	return vec4(-texture2D(tex_velocity, pos + offset).xy, 0, 1);
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy * (1.0 - 1.0 * dt);

	if(isMouseDown) {
		vec2 mRev = vec2(mouse.x, 1.0 - mouse.y);
		vec2 lmRev = vec2(lastMouse.x, 1.0 - lastMouse.y);
		vec2 mouseVel = -(lmRev - mRev) / dt;

		//v += 0.01 * mouseVel * dt * exp((pow(pos.x - lastMouse.x,  2) + pow(pos.y  - lastMouse.y, 2)) / 0.25);

		//float F = max(0.5 - length(vec2(lastMouse.x, 1.0 - lastMouse.y) - pos), 0);
		//v += dt * mouseVel * F;

		//float dist = (lastMouse.y - mouse.y) * pos.x - (lastMouse.x - mouse.x) * pos.y + lastMouse.x * mouse.y + mouse.x * lastMouse.y;
		//dist /= length(lastMouse - mouse);

		vec2 dm = mRev - lmRev;
		float lm = length(dm);

		float projection = dot(lmRev - pos, dm / lm);
		float fp = projection / lm;
		float dist;
		if(projection < 0.0){
			dist = length(pos - lmRev);
		}
		else if(projection > lm) {
			dist = length(pos - mRev);
		}
		else {
			dist = sqrt(abs(dot(pos - lmRev, pos - lmRev) - projection*projection));
		}

		float pf = 1.0 - clamp(fp, 0.0, 1.0) * 0.6;
		float m = exp(-dist / 0.025);

		m *= pf * pf;

		vec2 mv = pos - mRev;

		v += (mouseVel - v + mv) * m;

		//if(dot(mv,mv) < 0.01) {
		//	v += mv * dot(mv, mv) * 0.05 ;
		//}

		//float xDist = max(0.1 - abs(pos.x - mouse.x), 0);

		//v += vec2(mouseVel.y * xDist, xDist);
	}

	return vec4(v, 0, 1);
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
