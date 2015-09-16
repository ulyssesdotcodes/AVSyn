#version 330 core

uniform sampler2D tex_prev;
uniform sampler2D tex_audio;

uniform vec2 i_resolution;
uniform float i_fade;
uniform float i_highestVolume;
uniform float i_time;
uniform float i_accumulatedSound;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec2 toPolar(vec2 p, vec2 origin) {
  vec2 cuv = p - origin;

  float ca = atan(cuv.x, cuv.y) + radians(90.0);
  float cr = length(cuv);

  return vec2(ca, cr);
}

vec2 toCartesian(vec2 p) {
  return vec2(p.y * cos(p.x), p.y * sin(p.x));
}

vec2 rotate(vec2 pos) {
	pos = toPolar(pos, vec2(0.5));

	// Rotate
	pos.x -= 3.1415 * i_accumulatedSound * 0.04;
	return toCartesian(pos) + vec2(0.5);
}

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	vec2 posRotated = rotate(pos);

	vec3 prev = texture2D(tex_prev, pos).xyz * i_fade;

	vec2 audio = texture2D(tex_audio, vec2(posRotated.x, 0.25)).xy;

	float audX = audio.y * 0.4 + 0.5;
	float clamped = clamp(20 * (0.05 - abs(posRotated.y - audX)), 0, 1);

	fragColor = vec4(prev + hsv2rgb(vec3(i_highestVolume, 0.5, clamped)), 1);
}
