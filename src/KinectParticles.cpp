#include "KinectParticles.h"

#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int NUM_PARTICLES = 600e3;


void KinectParticles::setup(AudioSource* audioSource, BeatDetector* beatDetector, map<string, Visualization*> visualizations, vector<string> visualizationOptions)
{
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;

	mVisualizationOptions = visualizationOptions;
	mVisualizations = visualizations;
	mVisualization = mVisualizations.begin()->second;
	mCurrentVisOption = 0;

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

	mUpdateProg->uniform("positionRes", vec2(particlesImageWidth * 0.25, particlesImageHeight * 0.25));

	mDevice = Kinect2::Device::create();
	mDevice->start();
	mDevice->connectBodyIndexEventHandler( [ & ]( const Kinect2::BodyIndexFrame& frame )
	{
		mChannelBodyImage = frame.getChannel();
	} );

	mVisFbo = gl::Fbo::create(app::getWindowWidth(), app::getWindowHeight());
}

void KinectParticles::mouseMove(app::MouseEvent mouseEvent) 
{
	//gl::ScopedFramebuffer fbScp(mMousePositionFbo);
	//gl::clear(Color(0, 0, 0));
	//gl::ScopedViewport scpVp(ivec2(0), mMousePositionFbo->getSize());
	//gl::setMatricesWindow(mMousePositionFbo->getSize());

	//float x = (float)mouseEvent.getX();
	//float y = (float)mouseEvent.getY();
	//Rectf rect(x, y, x + 500, y + 500);
	//gl::ScopedGlslProg shaderProg(gl::getStockShader(gl::ShaderDef().color()));
	//gl::color(Color::white());
	//gl::drawSolidRect(rect);
}

void KinectParticles::switchCamera(CameraPersp* cam) 
{
	mCam = cam;
	cam->lookAt(vec3(0, 0, 100), vec3(0, 0, 0));
}

void KinectParticles::switchParams(params::InterfaceGlRef params, const string &group)
{
	mParams = params;
	addParamName(group + "/Kinect Visualizations");
	params->addParam(group + "/Kinect Visualizations", mVisualizationOptions,
		[=](int ind) {
			mVisualization->resetParams(mParams);
			mCurrentVisOption = ind;
			mVisualization = mVisualizations[mVisualizationOptions[mCurrentVisOption]];
			mVisualization->switchCamera(mCam);
			mVisualization->switchParams(mParams, "Kinect Aux");
		},
			[=]() {
			return mCurrentVisOption;
		})
		.group(group);
}

bool KinectParticles::perspective()
{
	return true;
}

void KinectParticles::update()
{
	float currentBeat = mAudioSource->getVolume();
	if (currentBeat - mLastBeat > 0.4) {
		mCurrentImage = (mCurrentImage + 1) % mImages.size();
		mUpdateProg->uniform("change", true);
	}

	mLastBeat = currentBeat;

	gl::TextureRef tex;

	if(mChannelBodyImage) {
		tex = gl::Texture::create(*mChannelBodyImage  );
	}
	else {
		tex = gl::Texture::create(Channel16u());
	}

	{
		mVisualization->update();
		gl::ScopedFramebuffer fbo(mVisFbo);
		gl::clear(Color(0, 0, 0));
		gl::ScopedViewport scpVp(ivec2(0), mVisFbo->getSize());
		gl::pushMatrices();
		if (mVisualization->perspective()) {
			mVisualization->switchCamera(mCam);
			gl::setMatrices(*mCam);
			switchCamera(mCam);
		}
		else {
			gl::setMatricesWindow(mVisFbo->getSize());
		}
		mVisualization->draw();
		gl::popMatrices();
	}

	gl::ScopedTextureBind texBind(tex, 1);
	gl::ScopedTextureBind bind(mVisFbo->getColorTexture(), 0);
	mUpdateProg->uniform("image", 0);
	mUpdateProg->uniform("time", (float)app::getElapsedSeconds());
	mUpdateProg->uniform("collisionRes", tex->getSize());
	mUpdateProg->uniform("collisionMap", 1);

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

void KinectParticles::draw() 
{
	if(mChannelBodyImage) {
		gl::TextureRef tex = gl::Texture::create( *mChannelBodyImage);
		gl::ScopedTextureBind bindCollision(tex, 1);
		gl::pushViewport();
		gl::viewport(app::getWindowSize());
		gl::pushMatrices();
		gl::setMatricesWindow(app::getWindowSize());
		gl::draw(tex, Rectf(0, 0, 256, 256));
		gl::popMatrices();
		gl::popViewport();
	}

	gl::ScopedGlslProg render(mRenderProg);
	gl::ScopedVao vao(mAttributes[mIteration & 1]);
	gl::setDefaultShaderVars();
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

