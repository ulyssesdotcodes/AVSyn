#version 330 core

uniform sampler2D tex_prev;
uniform sampler2D tex_audio;

uniform vec2 i_resolution;
uniform float i_fade;
uniform float i_highestVolume;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;

	vec3 prev = texture2D(tex_prev, pos).xyz * i_fade;

	vec2 audio = texture2D(tex_audio, vec2(pos.x, 0.25)).xy;

	float audX = audio.y * 0.3 + 0.5;
	float clamped = clamp(20 * (0.05 - abs(pos.y - audX)), 0, 1);

	fragColor = vec4(prev + hsv2rgb(vec3(i_highestVolume, 0.5, clamped)), 1);
}