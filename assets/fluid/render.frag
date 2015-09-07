#version 330 core
uniform sampler2D tex_smoke;

uniform vec2 resolution;

out vec4 fragColor;

void main() {
    vec2 position = gl_FragCoord.xy / resolution.xy;
	vec4 color = texture2D(tex_smoke, position);

	fragColor = color;
}