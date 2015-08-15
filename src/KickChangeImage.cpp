#include "KickChangeImage.h"

#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int NUM_PARTICLES = 600e3;


void KickChangeImage::setup(AudioSource* audioSource, BeatDetector* beatDetector) 
{
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;

	mCurrentImage = 0;
	mImages.push_back(gl::Texture::create(loadImage(app::loadAsset("N4GII.jpg"))));
	mImages.push_back(gl::Texture::create(loadImage(app::loadAsset("nightclub.jpg"))));

	mParticles.assign(NUM_PARTICLES, Particle());
	float imageScale = math<float>::sqrt(NUM_PARTICLES / mImages[mCurrentImage]->getAspectRatio());
	float particlesImageWidth = imageScale * mImages[mCurrentImage]->getAspectRatio();
	float imageWidthScale = mImages[mCurrentImage]->getWidth() / particlesImageWidth;
	float imageHeightScale = mImages[mCurrentImage]->getHeight() / (NUM_PARTICLES / particlesImageWidth);
	float halfParticlesImageWidth = particlesImageWidth * 0.5;
	float particlesImageHeight = NUM_PARTICLES / particlesImageWidth;

	for (int i = 0; i < NUM_PARTICLES; ++i) {
		float x = (i % (int) particlesImageWidth - halfParticlesImageWidth) * 0.25;
		float y = (i / particlesImageWidth - particlesImageHeight * 0.5) * 0.25;

		int imageX = particlesImageWidth - i % (int)particlesImageWidth;
		int imageY = particlesImageHeight - i / particlesImageWidth;

		auto &p = mParticles.at(i);
		p.pos = vec3(x, y, 0);
		p.vel = Rand::randVec3();
		p.home = p.pos;
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		p.color = vec4(1.0);
		p.ref = vec2(1.0) - vec2((float)imageX / particlesImageWidth, (float)imageY / particlesImageHeight);
	}

	mParticleBuffer[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.data(), GL_STATIC_DRAW);
	mParticleBuffer[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), nullptr, GL_STATIC_DRAW);
	
	mRenderProg = gl::getStockShader(gl::ShaderDef().color());
	
	for( int i = 0; i < 2; ++i )
	{	// Describe the particle layout for OpenGL.
		mAttributes[i] = gl::Vao::create();
		gl::ScopedVao vao( mAttributes[i] );
		
		// Define attributes as offsets into the bound particle buffer
		gl::ScopedBuffer buffer( mParticleBuffer[i] );
		gl::enableVertexAttribArray( 0 );
		gl::enableVertexAttribArray( 1 );
		gl::enableVertexAttribArray( 2 );
		gl::enableVertexAttribArray( 3 );
		gl::enableVertexAttribArray( 4 );
		gl::enableVertexAttribArray( 5 );
		gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
		gl::vertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
		gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
		gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, home) );
		gl::vertexAttribPointer( 4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, damping) );
		gl::vertexAttribPointer( 5, 2, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, ref) );
	}

	mUpdateProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(app::loadAsset("kick_image.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings({ "position", "color", "velocity", "home", "damping", "ref" })
		.attribLocation("iPosition", 0)
		.attribLocation("iColor", 1)
		.attribLocation("iVelocity", 2)
		.attribLocation("iHome", 3)
		.attribLocation("iDamping", 4)
		.attribLocation("iRef", 5)
		);
}

void KickChangeImage::switchCamera(CameraPersp* cam) 
{
	cam->lookAt(vec3(0, 0, 200), vec3(0));
}

void KickChangeImage::switchParams(params::InterfaceGlRef params)
{
	params->addButton("Push", [=]() {
		mCurrentImage = (mCurrentImage + 1) % mImages.size();
		mUpdateProg->uniform("change", true);
	});

}

bool KickChangeImage::perspective()
{
	return true;
}

void KickChangeImage::update()
{
	mBeatDetector->update(1.9);
	float currentBeat = mBeatDetector->getBeat();
	if (currentBeat - mLastBeat > 0.9) {
		mCurrentImage = (mCurrentImage + 1) % mImages.size();
		mUpdateProg->uniform("change", true);
	}

	mLastBeat = currentBeat;

	gl::ScopedTextureBind bind(mImages[mCurrentImage], 0);
	mUpdateProg->uniform("image", 0);
	mUpdateProg->uniform("time", (float)app::getElapsedSeconds());
	gl::ScopedGlslProg prog(mUpdateProg);
	gl::ScopedState rasterizer(GL_RASTERIZER_DISCARD, true);

	gl::ScopedVao source(mAttributes[mIteration & 1]);

	mIteration++;

	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mIteration & 1]);
	gl::beginTransformFeedback(GL_POINTS);

	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::endTransformFeedback();

	mUpdateProg->uniform("change", false);
}

void KickChangeImage::draw() 
{
	gl::ScopedGlslProg render(mRenderProg);
	gl::ScopedVao vao(mAttributes[mIteration & 1]);
	gl::setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

