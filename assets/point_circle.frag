in vec3 oColor;

out vec4 fragColor;

void main() {
  vec3 color = oColor;
  color = color * (1.0 - length(gl_PointCoord - vec2(0.5)) * 2.0);
  fragColor = vec4(color, 1.0);
}