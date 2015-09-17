uniform mat4 ciModelViewProjection;
uniform vec3 i_eqs;
uniform vec3 i_eq0;
uniform vec3 i_eq1;
uniform vec3 i_eq2;
uniform float i_hue;

in vec4 ciPosition;

out vec3 vColor;

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

void main() {
  float size = 1.0;
  float power = 1.2;
  float loudness = 128.0;

  float r = i_eqs.r * loudness / pow(length(ciPosition.xyz - i_eq0), power);
  float g = i_eqs.g * loudness / pow(length(ciPosition.xyz - i_eq1), power);
  float b = i_eqs.b * loudness / pow(length(ciPosition.xyz - i_eq2), power);

  vec3 hsv = rgb2hsv(vec3(r, g, b)); 
  hsv.x = fract(hsv.x + i_hue);
  hsv.x = fract(hsv.x * 2.0);
  hsv.z = fract(hsv.z * 2.0);
  vColor = hsv2rgb(hsv);

  gl_PointSize = 1.0;
  gl_Position = ciModelViewProjection * vec4(ciPosition * size);
}