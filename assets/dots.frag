uniform vec2 i_resolution;
uniform float i_time;
uniform float i_accumulatedLoudness;
uniform float i_eqs[4];
uniform float i_hue;

out vec4 fragColor;

vec2 toCartesian(vec2 p) {
  return vec2(p.y * cos(p.x), p.y * sin(p.x));
}

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

float iDot(float section, vec2 uv) {

  float l = 0.0;
  if (section == 0.0) {
    l = i_eqs[0];
  }
  else if (section == 1.0) {
    l = i_eqs[1];
  }
  else if (section == 2.0) {
    l = i_eqs[2];
  }
  else if (section == 3.0){
    l = i_eqs[3];
  }
  else {
    l = 0.0;
  }

  vec2 cuv = vec2(i_accumulatedLoudness / (64.0 / (section * 0.5 + 1.0)) , cos(i_time + section * 0.75));
  vec2 center = toCartesian(cuv);
  float dist = length(center - uv);

  return smoothstep(l * i_resolution.x * 0.001, 0.0, dist * dist);
}

void main() {
  vec2 uv = ((2.0 * gl_FragCoord.xy) / i_resolution.xy - vec2(1.0)) * vec2(i_resolution.x/i_resolution.y, 1.0);

  vec3 col = vec3(0.0);
  for(float i = 0.0; i < 4.0; i++) {
    vec3 color = hsv2rgb(vec3(fract(i_hue + i * 0.25), 1.0, 1.0));

    col += iDot(i, uv) * color ;
  }

  fragColor = vec4(col, 1.0);
}