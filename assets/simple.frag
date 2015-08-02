precision highp float;

in ciPosition;

void main() {
	vec2 uv = gl_FragCoord.xy / resolution.xy;
	float fft = texture2D(audioTexture, vec2(uv.x, 0.25)).r;
	float visibility = ceil(fft - vUv.y);
	vec3 freq = vec3(visibility);

	gl_FragColor = vec4(freq, 1.0);
}