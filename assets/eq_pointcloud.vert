uniform mat4 ciModelViewProjection;
uniform vec3 i_eqs;
uniform vec3 i_eq0;
uniform vec3 i_eq1;
uniform vec3 i_eq2;

in vec4 ciPosition;

out vec3 vColor;

void main() {
  float size = 1.0;
  float power = 1.2;
  float loudness = 128.0;
  

  vColor.r = i_eqs.r * loudness / pow(length(ciPosition.xyz - i_eq0), power);
  vColor.g = i_eqs.g * loudness / pow(length(ciPosition.xyz - i_eq1), power);
  vColor.b = i_eqs.b * loudness / pow(length(ciPosition.xyz - i_eq2), power);

  gl_PointSize = 1.0;
  gl_Position = ciModelViewProjection * vec4(ciPosition * size);
}