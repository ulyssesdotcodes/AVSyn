uniform mat4 ciModelViewProjection;
uniform sampler2D uPositionTexture;

in vec4 ciPosition;
in vec2 texCoord;

//out float pointSize;
out vec3 vColor;
//out vec4 position;

vec3 hsv2rgb(vec3 c) {
  vec4 K = vec4(1.0, 2.0 / 3.0, 1.0 / 3.0, 3.0);
  vec3 p = abs(fract(c.xxx + K.xyz) * 6.0 - K.www);
  return c.z * mix(K.xxx, clamp(p - K.xxx, 0.0, 1.0), c.y);
}

void main() {
	vec4 pos = texture2D(uPositionTexture, texCoord);
	//color = vec3(1.0, 1.0, 1.0);
	vec4 mvPosition = vec4(vec3(texCoord, 0.0), 1.0);
	//position = mvPosition;
	//pointSize = 500.0 / length(mvPosition.xyz);
	vColor = vec3(1.0);
	//pointSize = 1.0;
	gl_PointSize = 1.0;
	gl_Position = ciModelViewProjection * mvPosition;
}