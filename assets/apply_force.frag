#version 330 core

uniform int WIDTH;
uniform int HEIGHT;

uniform vec2 resolution;
uniform sampler2D tex_velocity;

uniform float time;
uniform float dt;

uniform bool isMouseDown;
uniform ivec2 mouse;
uniform ivec2 lastMouse;

out vec4 fragColor;

vec4 boundary(vec2 pos) {
	vec4 outVel;
	if(pos.x == 0) {
		outVel = texture2D(tex_velocity, vec2(0 + 1./WIDTH, pos.y));
	}
	else if(pos.y == 0) {
		outVel = texture2D(tex_velocity, vec2(pos.x, 0 + 1./HEIGHT));
	}
	else if(floor(pos.x * WIDTH) == WIDTH - 1) {
		outVel = texture2D(tex_velocity, vec2(1.0 - 1./WIDTH, pos.y));
	}
	else if(floor(pos.y * HEIGHT) == HEIGHT - 1) {
		outVel = texture2D(tex_velocity, vec2(pos.x, 1.0 - 1./HEIGHT));
	}

	return -outVel;
}

vec4 inner(vec2 pos) {
	vec4 velocity = texture2D(tex_velocity, pos);
	vec2 v = velocity.xy * (1.0 - 1.0 * dt);

	if(isMouseDown) {
		vec2 mouseVel = -(lastMouse - mouse) / dt;

		v += mouseVel * dt * exp((pow((pos.x - lastMouse.x / resolution.x) / WIDTH,  2) + pow((pos.y - lastMouse.y / resolution.y) / HEIGHT, 2)) / 0.1);

  //      float x = mod(iConnections[0] + 1, WIDTH);
		//int y = (iConnections[0] + 1) / WIDTH;

		//float F = max(100 - length(mouse - vec2(x, y)), 0);
		//v += dt * mouseVel * F;
	}

	return vec4(v, 0, 1);
}

void main() {
	vec4 outVelocity;
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	if(pos.x == 0 || pos.y == 0 || floor(pos.x * WIDTH) == WIDTH - 1 || floor(pos.y * HEIGHT) == HEIGHT - 1) {
		outVelocity = boundary(pos);
	}
	else {
		outVelocity = inner(pos);
	}

	tf_velocity = outVelocity;
}
