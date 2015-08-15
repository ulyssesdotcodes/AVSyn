uniform bool change;
uniform sampler2D image;
uniform float time;

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

	if(change) {
		velocity = 2.0 * (vec3(0.5) - vec3(rand(ref+time), rand(ref-time), rand(ref*time)));
	}

	position = position +  velocity;
	vec3 dir = normalize(home - position);
	velocity = velocity * damping + dir * 64.0 / (60.0 * 60.0);
}