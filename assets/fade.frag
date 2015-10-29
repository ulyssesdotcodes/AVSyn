#version 330 core

uniform sampler2D tex_prev;

uniform vec2 i_resolution;
uniform float i_fade;

out vec4 fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;

	vec3 prev = texture2D(tex_prev, pos).xyz * i_fade;

	fragColor = vec4(prev, 1);
}
