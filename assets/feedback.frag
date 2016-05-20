#version 330 core

uniform sampler2D tex_prev;
uniform sampler2D tex_current;

uniform vec2 i_resolution;
uniform float i_fade;
uniform float i_scale;
uniform float i_offsetY;
uniform float i_manipFade;
uniform float i_hueShift;
uniform float i_saturationShift;
uniform float i_lightnessShift;
uniform float i_beat;
uniform float i_beatExpand;
uniform float i_beatRotate;

out vec4 fragColor;

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

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	pos -= (pos - vec2(0.5)) * i_beat * i_beatExpand;
	//pos = rotate(pos);
	vec3 current = clamp(texture2D(tex_current, pos).xyz, vec3(0.0), vec3(0.999));

	vec3 hsv = rgb2hsv(current);

	hsv = vec3(fract(hsv.x + i_hueShift), clamp(hsv.y - i_saturationShift, 0.001, 0.999), clamp(hsv.z * i_lightnessShift, 0.001, 0.999));
	current = hsv2rgb(hsv);

	vec2 prevPos = pos;
	vec3 prev = vec3(0);

	prevPos = (prevPos - 0.5 + 0.5 * i_scale) / i_scale;
	prevPos.y = prevPos.y + i_offsetY;
	if(prevPos.x > 0 && prevPos.x < 1.0 && prevPos.y > 0 && prevPos.y < 1.0) {
		prev = clamp(texture2D(tex_prev, prevPos).xyz * i_manipFade + texture2D(tex_prev, pos).xyz * i_fade, 0, 1);
	}
	else {
	    prev = vec3(0);
	}

	fragColor = vec4(clamp(prev + current, 0, 1), 1);
}
