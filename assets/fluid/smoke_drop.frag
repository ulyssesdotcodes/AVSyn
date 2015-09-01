#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_prev;

uniform vec2 smokeDropPos;
uniform float time;
uniform float dt;

out vec4 fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec3 current = texture2D(tex_prev, pos).xyz;

	vec2 dropDistance = pos - vec2(smokeDropPos.x, 1.0 - smokeDropPos.y);

	vec3 smoke = vec3(max(0, 0.005 - dot(dropDistance, dropDistance))) * dt * 256.0; 

	fragColor = vec4(current + smoke, 1);
}