#version 330 core

uniform vec3 resolution;
uniform sampler2D tex_audio;
uniform sampler2D tex_prev;

uniform float i_fade;

out vec4 fragColor;

void main() {
	vec2 pos = gl_FragCoord.xy / resolution.xy;

	vec4 prev = texture2D(tex_prev, pos) * i_fade;

	float audX = texture2D(tex_audio, vec2(pos.x, 0.25)).g+ 0.5;

	vec4 newColor = vec4(vec3(clamp(0, 1, 20 * (0.05 - abs(pos.y - audX)))), 1);

	fragColor = vec4(1);//prev + newColor;
}