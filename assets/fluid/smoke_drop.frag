#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_prev;

uniform vec2 smokeDropPos;
uniform float volume;
uniform float beat;
uniform float dt;

out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec3 current = texture2D(tex_prev, pos).xyz * 0.99;

	vec2 mSDP = vec2(smokeDropPos.x, 1.0 - smokeDropPos.y);

	//mSDP.y *= 0.5;
	//mSDP.y += 0.5 * volume;

	vec2 dropDistance = pos - mSDP;

	vec3 smoke = vec3(max(0, 0.004 - dot(dropDistance, dropDistance))) * dt * 1024;// * beat;

	smoke *= mix(0.8, 1.0, rand(vec2(pos.x * pos.y, cos(dt + volume))));

	fragColor = vec4(current + smoke, 1);
}