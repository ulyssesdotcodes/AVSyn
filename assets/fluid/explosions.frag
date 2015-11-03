#version 330 core

uniform vec2 i_resolution;
uniform sampler2D tex_prev;

uniform vec2 i_smokeDropPos;
uniform float i_intensity;
uniform float i_time;
uniform float i_dt;

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
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	vec3 current = texture2D(tex_prev, pos).xyz;
	current.x *= (1.0 - 0.8 * i_dt);
	current.y *= (1.0 - 0.05 * i_dt);

	if(i_intensity < 0.001) {
		fragColor = vec4(current, 1.0);
		return;
	}

	vec2 dropDistance = pos - i_smokeDropPos;

	float density = max(0, 0.016 - dot(dropDistance, dropDistance)) * i_dt * i_intensity;

	density *= mix(0.6, 1.0, rand(vec2(pos.x * pos.y, cos(i_time))));

	float temperature = current.y + density * 32;

	float hue = current.z;
	if(density > 0.0001) {
		hue = 0.10;
	}

	fragColor = vec4(current.x + density, temperature, hue, 1);
}
