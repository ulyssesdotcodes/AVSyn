#version 330 core


uniform sampler2D tex_A;
uniform sampler2D tex_B;

uniform vec2 i_resolution;
uniform float i_add;
uniform float i_multiply;
uniform float i_fade;
uniform float i_time;
uniform float i_beat;
uniform float i_beatExpand;
uniform float i_beatRotate;
uniform float i_beatLight;

out vec4 o_fragColor;

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
	pos.x -= 3.1415 * i_beat * i_beatRotate;
	return toCartesian(pos) + vec2(0.5);
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	pos -= (pos - vec2(0.5)) * i_beat * i_beatExpand;
	pos = rotate(pos);
	vec4 A = (1.0 - i_fade) * texture2D(tex_A, pos);
	vec4 B = i_fade * texture2D(tex_B, pos);

	vec4 color = i_multiply * A * B + i_add * (A + B);

	vec3 hsv = rgb2hsv(color.xyz);
	hsv.z += hsv.z * i_beat * i_beatLight;
	o_fragColor = vec4(hsv2rgb(hsv), 1);
}