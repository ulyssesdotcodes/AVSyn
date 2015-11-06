#version 330 core

uniform sampler2D tex_prev;
uniform sampler2D tex_current;

uniform vec2 i_resolution;
uniform float i_fade;
uniform float i_scale;
uniform float i_scaleFade;
uniform bool i_discardCurrent;

out vec4 fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	vec3 current = clamp(texture2D(tex_current, pos).xyz, 0, 1);

	vec2 prevPos = (pos - 0.5 + 0.5 * i_scale) / i_scale;
	vec3 prev = vec3(0);

	if(abs(i_scale - 1.0) > 0.001 && prevPos.x > 0 && prevPos.x < 1.0 && prevPos.y > 0 && prevPos.y < 1.0) {
		prev = texture2D(tex_prev, prevPos).xyz * i_scaleFade;
	}

	if(i_fade > 0.0) {
		prev += texture2D(tex_prev, pos).xyz * i_fade;
	}

	prev = clamp(prev, 0, 1);

	fragColor = vec4(clamp(prev + current, 0, 1), 1);
}
