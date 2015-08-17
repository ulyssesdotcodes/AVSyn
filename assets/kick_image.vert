uniform bool change;
uniform sampler2D image;
uniform sampler2D collisionMap;
uniform float time;
uniform ivec2 collisionRes;
uniform vec2 positionRes;

in vec3   iPosition;
in vec4   iColor;
in vec3   iVelocity;
in vec3   iHome;
in float  iDamping;
in vec2 iRef;

out vec3  position;
out vec4  color;
out vec3  velocity;
out vec3  home;
out float damping;
out vec2 ref;

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
	position = iPosition;
	color = iColor;
	velocity = iVelocity;
	damping = iDamping;
	home = iHome;
	ref = iRef;

		//float imageScale = math<float>::sqrt(NUM_PARTICLES / mImages[mCurrentImage]->getAspectRatio());
		//float particlesImageWidth = imageScale * mImages[mCurrentImage]->getAspectRatio();
		//float imageWidthScale = mImages[mCurrentImage]->getWidth() / particlesImageWidth;
		//float imageHeightScale = mImages[mCurrentImage]->getHeight() / (NUM_PARTICLES / particlesImageWidth);
		//float halfParticlesImageWidth = particlesImageWidth * 0.5;
		//float particlesImageHeight = NUM_PARTICLES / particlesImageWidth;

	color = texture2D(image, ref);
	//color = color + texture2D(collisionMap, position.xy);

	if(change) {
		velocity = 2.0 * (vec3(0.5) - vec3(rand(ref+time), rand(ref-time), rand(ref*time)));
	}

	position = position + velocity;

	vec2 kinPos = mix(vec2(0.1), vec2(0.9), position.xy / positionRes) + vec2(0.5);
	kinPos.x = 1.0 - kinPos.x;
	vec4 collision = texture2D(collisionMap, kinPos);

	vec3 dir = normalize(home - position);

	// If the target position is a collision, move away instead of towards but keep velocity.
	if (collision.x + collision.y + collision.z < 2.9) {
		position = position - vec3(dir.xy, 0.0);
	}

	velocity = velocity * damping + dir * 64.0 / (60.0 * 60.0);
}