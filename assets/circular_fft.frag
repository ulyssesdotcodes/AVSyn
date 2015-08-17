uniform vec3 resolution;
uniform sampler2D audioTexture; 
uniform float time;

out vec4 fragColor;

vec2 toPolar(vec2 p, vec2 origin) {
  vec2 cuv = p - origin;

  float ca = atan(cuv.x, cuv.y) + radians(90.0);
  float cr = length(cuv);

  return vec2(ca, cr);
}

vec4 fromPos(vec2 uv, vec3 te) {
    vec2 cuv = toPolar(uv, te.xy);

    // FFT
    float fft = texture2D(audioTexture, vec2(cuv.y * 0.33, 0.25)).x;

    // Rotating colors
    float fftSin = (0.5 + 0.3 * sin(cuv.y * 64.0 * 3.1415)) * fft * fft;
    fftSin = smoothstep(0.0, 1.0, fftSin) * 3.0;
    return vec4(sin(fftSin * 3.1415 * 2.5), sin(fftSin * 3.1415 * 2.0), sin(fftSin * 3.1415 * 1.0), 1.0);
}

void main(void)
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy - vec2(0.5)) * vec2(resolution.x/resolution.y, 1.0);

    vec4 color = vec4(fromPos(uv, vec3(0.0)).rbg, 1.0);

    fragColor = color;
}