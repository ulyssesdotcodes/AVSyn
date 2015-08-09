in vec3 oColor;
in vec2 oVelocity;
in float pointSize;

out vec4 fragColor;

void main() {
  vec2 velocity = normalize(oVelocity);
  vec2 pointCoord = gl_PointCoord;
  pointCoord.x = pointCoord.x - 0.5 + velocity.x * 0.5;
  pointCoord.y = 1.0 - pointCoord.y;
  float dist = abs(velocity.y * pointCoord.x - velocity.x * pointCoord.y) / sqrt(dot(velocity, velocity));
  if(dist > 0.01) discard;
  vec3 color = oColor * (1.0 - dist);
  fragColor = vec4(color, 1.0);
}