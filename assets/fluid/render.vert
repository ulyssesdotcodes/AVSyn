#version 330 core

uniform int WIDTH;
uniform int HEIGHT;
uniform samplerBuffer tex_velocity;

uniform mat4 ciModelView;
uniform mat4 ciProjectionMatrix;

in vec4 ciPosition;

out vec4 oColor;

void main() {
	vec2 velPos = vec2(ciPosition.x * float(WIDTH), ciPosition.y * float(HEIGHT));
	vec4 vel = texelFetch(tex_velocity, int(mod(velPos, WIDTH) + velPos * WIDTH));
	oColor = vec4(vel.xy, 0, 1);

	gl_Position = ciProjectionMatrix * ciModelView * ciPosition;
}