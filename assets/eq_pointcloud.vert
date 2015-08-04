uniform mat4 ciModelViewProjection;
uniform vec3 eqs;
uniform vec3 eq0;
uniform vec3 eq1;
uniform vec3 eq2;

in vec4 ciPosition;

out vec3 vColor;

void main() {
  float size = 1.0;
  float power = 1.2;
  float loudness = 128.0;
  float r = eqs.r * loudness / pow(length(ciPosition.xyz - eq0), power);
  float g = eqs.g * loudness / pow(length(ciPosition.xyz - eq1), power);
  float b = eqs.b * loudness / pow(length(ciPosition.xyz - eq2), power);
  vColor = vec3(r, g, b);
  gl_PointSize = 1.0;
  gl_Position = ciModelViewProjection * vec4(ciPosition * size);
}