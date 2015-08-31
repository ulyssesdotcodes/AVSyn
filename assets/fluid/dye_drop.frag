#version 330 core

uniform vec2 resolution;
uniform sampler2D tex_prev;

uniform bool isMouseDown;
uniform vec2 mouse;
uniform vec2 lastMouse;

uniform float time;

out vec4 fragColor;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec3 dyeC = texture2D(tex_prev, pos).xyz;

	vec2 mouseRev = vec2(mouse.x, 1.0 - mouse.y);

	if(isMouseDown) {
		float d = length(mouseRev - pos);

		if(d < 0.01) {
			vec2 mRev = vec2(mouse.x, 1.0 - mouse.y);
			vec2 lmRev = vec2(lastMouse.x, 1.0 - lastMouse.y);

			vec2 dm = mRev - lmRev;
			float lm = length(dm);

			float projection = dot(lmRev - pos, dm / lm);
			float fp = projection / lm;
			float dist;

			dist = sqrt(abs(dot(pos - lmRev, pos - lmRev) - projection*projection));

			dyeC = hsv2rgb(vec3(1.0, mix(0, 1, 1.0 - dist * 20), 1.0));
		}
	}

	fragColor = vec4(dyeC, 1);
}