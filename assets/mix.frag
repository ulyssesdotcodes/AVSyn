#version 330 core


uniform sampler2D tex_A;
uniform sampler2D tex_B;

uniform vec2 i_resolution;
uniform float i_add;
uniform float i_multiply;
uniform float i_fade;

out vec4 o_fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / i_resolution.xy;
	vec4 A = i_fade * texture2D(tex_A, pos);
	vec4 B = (1.0 - i_fade) * texture2D(tex_B, pos);

	o_fragColor = i_multiply * A * B + i_add * (A + B);
}