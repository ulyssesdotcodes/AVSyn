#include "KickChangeImage.h"

#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int NUM_PARTICLES = 600e3;


void KickChangeImage::setup(AudioSource* audioSource) 
{
	mAudioSource = audioSource;

	mCurrentImage = Surface32f::create(loadImage(app::loadAsset("N4GII.jpg")));

	mParticles.assign(NUM_PARTICLES, Particle());
	float imageScale = math<float>::sqrt(NUM_PARTICLES / mCurrentImage->getAspectRatio());
	float particlesImageWidth = imageScale * mCurrentImage->getAspectRatio();

	for (int i = 0; i < NUM_PARTICLES; ++i) {

		float x = (i % (int) particlesImageWidth - particlesImageWidth * 0.5) * 0.25;
		float y = (i / particlesImageWidth - (NUM_PARTICLES / particlesImageWidth) * 0.5) * 0.25;

		int imageX = particlesImageWidth - i % (int)particlesImageWidth;
		int imageY = NUM_PARTICLES / particlesImageWidth - i / particlesImageWidth;

		auto &p = mParticles.at(i);
		p.pos = vec3(x, y, 0);
		p.vel = Rand::randVec3();
		p.home = p.pos;
		p.damping = Rand::randFloat( 0.965f, 0.985f );
		p.color = mCurrentImage->getPixel(ivec2(imageX, imageY));
	}

	mParticleBuffer[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), mParticles.data(), GL_DYNAMIC_DRAW);
	mParticleBuffer[1] = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles.size() * sizeof(Particle), nullptr, GL_DYNAMIC_DRAW);
	
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
		gl::vertexAttribPointer( 0, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, pos) );
		gl::vertexAttribPointer( 1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color) );
		gl::vertexAttribPointer( 2, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, vel) );
		gl::vertexAttribPointer( 3, 3, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, home) );
		gl::vertexAttribPointer( 4, 1, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, damping) );
	}

	mUpdateProg = gl::GlslProg::create(gl::GlslProg::Format().vertex(app::loadAsset("kick_image.vert"))
		.feedbackFormat(GL_INTERLEAVED_ATTRIBS)
		.feedbackVaryings({ "position", "velocity", "home", "color", "damping" })
		.attribLocation("iPosition", 0)
		.attribLocation("iColor", 1)
		.attribLocation("iVelocity", 2)
		.attribLocation("iHome", 3)
		.attribLocation("iDamping", 4)
		);
}

void KickChangeImage::switchCamera(CameraPersp* cam) 
{
	cam->lookAt(vec3(0, 0, 200), vec3(0));
}

void KickChangeImage::switchParams(params::InterfaceGlRef params)
{
	params->addButton("Push", [=]() {
		for (int i = 0; i < NUM_PARTICLES; ++i) {
			mParticles[i].vel = Rand::randVec3();
		}

		mParticleBuffer[mIteration & 1]->copyData(mParticles.size() * sizeof(Particle), mParticles.data());
	});

}

bool KickChangeImage::perspective()
{
	return true;
}

void KickChangeImage::update()
{
	mUpdateProg->uniform("volume", mAudioSource->getVolume());
	gl::ScopedGlslProg prog(mUpdateProg);
	gl::ScopedState rasterizer(GL_RASTERIZER_DISCARD, true);

	gl::ScopedVao source(mAttributes[mIteration & 1]);

	mIteration++;

	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mParticleBuffer[mIteration & 1]);
	gl::beginTransformFeedback(GL_POINTS);

	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::endTransformFeedback();
}

void KickChangeImage::draw() 
{
	gl::ScopedGlslProg render(mRenderProg);
	gl::ScopedVao vao(mAttributes[mIteration & 1]);
	gl::setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

