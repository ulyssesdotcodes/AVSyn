uniform vec2 i_resolution;
uniform sampler2D tex_audio; 
uniform float i_time;
uniform float i_hue;
uniform float i_volume;
uniform float i_holeSize;

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

void main(void)
{
    vec2 uv = (gl_FragCoord.xy / i_resolution.xy - vec2(0.5)) * vec2(i_resolution.x/i_resolution.y, 1.0);
    vec2 cuv = toPolar(uv, vec2(0));

	cuv.x = (cuv.x + 3.1415 * 0.5) * 2 / (2 * 3.1415) - 1.0;
	vec3 sound = texture2D(tex_audio, vec2(fract(cuv.x), 0.25)).xyz;
	if(cuv.y < i_holeSize) {
		discard;
	}

	vec3 color;
	color.x = cuv.x;
	color.y = 1.0;
	color.z = ceil(sound.x * 0.5 + i_holeSize - cuv.y);

    fragColor = vec4(hsv2rgb(color), 1.0);
}
