#version 330 core

uniform sampler2D tex_A;
uniform sampler2D tex_B;

uniform vec2 resolution;

out vec4 fragColor;

void main() {
    vec2 pos = gl_FragCoord.xy / resolution.xy;
	vec4 A = texture2D(tex_A, pos);
	vec4 B = texture2D(tex_B, pos);

	fragColor = A * B;
}
