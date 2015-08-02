precision highp float;

uniform vec3 iResolution;
uniform sampler2D iChannel0; 
uniform float iGlobalTime;

in vec2 ciPosition;

out vec4 fragColor;

void main() {
	vec2 uv = gl_FragCoord.xy / iResolution.xy;
	float fft = texture2D(iChannel0, vec2(uv.x, 0.25)).r;
	float visibility = ceil(fft - uv.y);
	vec3 freq = vec3(visibility);

	fragColor = vec4(freq, 1.0);
	//fragColor = vec4(1.0, 0.0, 0.0, 1.0);
}