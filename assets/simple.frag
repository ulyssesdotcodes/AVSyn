precision highp float;

uniform vec2 i_resolution;
uniform sampler2D tex_audio; 
uniform float i_time;

in vec2 ciPosition;

out vec4 fragColor;

void main() {
	vec2 uv = gl_FragCoord.xy / i_resolution.xy;
	float fft = texture2D(tex_audio, vec2(uv.x, 0.25)).r;
	float visibility = ceil(fft - uv.y);
	vec3 freq = vec3(visibility);

	fragColor = vec4(freq, 1.0);
}