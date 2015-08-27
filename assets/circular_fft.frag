uniform vec3 resolution;
uniform sampler2D audioTexture; 
uniform float time;
uniform float hue;
uniform float volume;

out vec4 fragColor;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

vec3 rgb2hsv(vec3 c) {
  vec4 K = vec4(0.0, -1.0 / 3.0, 2.0 / 3.0, -1.0);
  vec4 p = mix(vec4(c.bg, K.wz), vec4(c.gb, K.xy), step(c.b, c.g));
  vec4 q = mix(vec4(p.xyw, c.r), vec4(c.r, p.yzx), step(p.x, c.r));

  float d = q.x - min(q.w, q.y);
  float e = 1.0e-10;
  return vec3(abs(q.z + (q.w - q.y) / (6.0 * d + e)), d / (q.x + e), q.x);
}

vec2 toPolar(vec2 p, vec2 origin) {
  vec2 cuv = p - origin;

  float ca = atan(cuv.x, cuv.y) + radians(90.0);
  float cr = length(cuv);

  return vec2(ca, cr);
}

vec4 fromPos(vec2 uv, vec3 te) {
    vec2 cuv = toPolar(uv, te.xy);

    // FFT
    float fft = texture2D(audioTexture, vec2(cuv.y * 0.33, 0.25)).x * volume;

    // Rotating colors
    float fftSin = (0.5 + 0.3 * sin(cuv.y * 64.0 * 3.1415)) * fft * fft;
    fftSin = smoothstep(0.0, 1.0, fftSin) * 3.0;
    return vec4(sin(fftSin * 3.1415 * 2.5), sin(fftSin * 3.1415 * 2.0), sin(fftSin * 3.1415 * 1.0), 1.0);
}

void main(void)
{
    vec2 uv = (gl_FragCoord.xy / resolution.xy - vec2(0.5)) * vec2(resolution.x/resolution.y, 1.0);

    vec3 color = vec4(fromPos(uv, vec3(0.0)).rbg, 1.0).xyz;

	color = rgb2hsv(color);

	color.r = fract(color.r + hue);

	color = hsv2rgb(color);

    fragColor = vec4(color, 1.0);
}