uniform float volume;

in vec3   iPosition;
in vec3   iVelocity;
in vec3   iHome;
in float  iDamping;
in vec4   iColor;

out vec3  position;
out vec3  velocity;
out vec3  home;
out float damping;
out vec4  color;

void main() {
	position = iPosition;
	velocity = iVelocity;
	damping = iDamping;
	home = iHome;
	color = iColor;

	position = position + velocity;
		vec3 dir = normalize(home - position);
		velocity = velocity * damping + dir * 64.0 / (60.0 * 60.0);
}