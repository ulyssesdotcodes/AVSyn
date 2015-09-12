#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_prev;

uniform vec2 smokeDropPos;
uniform float volume;
uniform float beat;
uniform float dt;
uniform float hue;
uniform float saturation;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec3 current = texture2D(tex_prev, pos).xyz * 0.99;

	vec2 mSDP = vec2(smokeDropPos.x, 1.0 - smokeDropPos.y);

	vec2 dropDistance = pos - mSDP;

	float smoke = max(0, 0.008 - dot(dropDistance, dropDistance)) * dt * 4096 * max(beat, 0.125) * pow(volume, 0.5);

	smoke *= mix(0.6, 1.0, rand(vec2(pos.x * pos.y, cos(dt + volume))));

	fragColor = vec4(current + hsv2rgb(vec3(hue, saturation, 1.0)) * smoke, 1);
}