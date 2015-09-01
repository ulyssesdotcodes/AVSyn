#version 330 core
uniform sampler2D tex_dye;

uniform vec2 resolution;

out vec4 fragColor;

void main() {
    vec2 position = gl_FragCoord.xy / resolution.xy;
	vec4 color = texture2D(tex_dye, position);// * 100;

	fragColor = color;
}