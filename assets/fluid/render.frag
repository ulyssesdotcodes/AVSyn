#version 330 core
uniform int WIDTH;
uniform int HEIGHT;
uniform samplerBuffer tex_dye;

uniform vec3 resolution;

out vec4 fragColor;

void main() {
    vec2 position = gl_FragCoord.xy / resolution.xy;

	ivec2 velPos = ivec2(position.x * WIDTH, position.y * HEIGHT);
	vec4 vel = texelFetch(tex_dye, int(velPos.x + velPos.y * WIDTH));

	fragColor = vel;
}