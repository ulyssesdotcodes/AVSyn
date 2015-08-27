uniform int WIDTH;

in vec4 iVelocity;
in ivec4 iConnections;

uniform float dt;

out vec4 velocity;

vec4 boundary() {
	return vec4(0);
}

vec4 inner() {
	float x = float(mod(iConnections[0] - 1, WIDTH)) / float(WIDTH);
	float F = max(abs(0.5 - x) - 0.4, 0.0);
	return iVelocity + dt * vec2(0, 1) * F;
}

void main() {
	vec4 outVelocity;
	if(iConnections[0] == -1 || iConnections[1] == -1 || iConnections[2] == -1 || iConnections[3] == -1) {
		outVelocity = boundary();
	}
	else {
		outVelocity = inner();
	}

	tf_velocity = outVelocity;
}
