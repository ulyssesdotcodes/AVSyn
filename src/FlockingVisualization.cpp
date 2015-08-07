#include "cinder\app\App.h"
#include "cinder\Rand.h"
#include "cinder\Camera.h"
#include "FlockingVisualization.h"

using namespace ci;

const int SIZE = 50;
const int BUFFER_WIDTH = 64;
const int NUM_PARTICLES = BUFFER_WIDTH * BUFFER_WIDTH;

FlockingVisualization::FlockingVisualization()
{
}

void FlockingVisualization::setup(AudioSource audioSource)
{
	mAudioSource = audioSource;

	mStep = true;

	mUpdateShader = gl::GlslProg::create(app::loadAsset("passthru.vert"), app::loadAsset("flocking_update.frag"));
	mRenderShader = gl::GlslProg::create(app::loadAsset("flocking.vert"), app::loadAsset("color.frag"));

	setupPingPongFbo();
	setupVbo();

	mParticlesBatch = gl::Batch::create(geom::Rect(mParticlesFbo.getBounds()), mUpdateShader);
	mRenderBatch = gl::Batch::create(mVboMesh, mRenderShader);
}

void FlockingVisualization::switchCamera(CameraPersp cam) 
{
	cam.lookAt(vec3(0.0, 100.0, 400.0), vec3(0.0));
}

void FlockingVisualization::setupPingPongFbo() {
	vector<Surface32f> surfaces;
	surfaces.push_back(Surface32f(BUFFER_WIDTH, BUFFER_WIDTH, true));
	Surface32f::Iter pixelIter = surfaces[0].getIter();

	// Initial positions
	while (pixelIter.line()) {
		surfaces[0].setPixel(pixelIter.getPos(),
			ColorAf(SIZE * (Rand::randFloat() - 0.5f),
				SIZE * (Rand::randFloat() - 0.5f),
				SIZE * (Rand::randFloat() - 0.5f),
				1.0));
	}

	surfaces.push_back(Surface32f(BUFFER_WIDTH, BUFFER_WIDTH, true));

	// Initial velocities
	while (pixelIter.line()) {
		surfaces[0].setPixel(pixelIter.getPos(),
			ColorAf(0.0f, 0.0f, 0.0f, 1.0));
	}

	mParticlesFbo = PingPongFBO(surfaces);
}


void FlockingVisualization::setupVbo() 
{
	vector<vec2> texCoords(NUM_PARTICLES);
	vector<_Uint32t> indices(NUM_PARTICLES);
	geom::BufferLayout layout;
	layout.append(geom::TEX_COORD_0, 2, 0, 0);

	for (int x = 0; x < BUFFER_WIDTH; ++x) {
		for (int y = 0; y < BUFFER_WIDTH; ++y) {
			indices.push_back(x * BUFFER_WIDTH + y);
			texCoords.push_back(vec2(x / (float)BUFFER_WIDTH, y / (float)BUFFER_WIDTH));
		}
	}

	gl::VboRef indicesVbo = gl::Vbo::create(GL_ARRAY_BUFFER, indices.size(), indices.data());
	auto texCoordsVbo = gl::Vbo::create(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data());
	mVboMesh = gl::VboMesh::create(texCoords.size(), GL_POINTS, { {layout, texCoordsVbo} });
}


void FlockingVisualization::update()
{
	if (true) {
		CameraPersp cam((int)mParticlesFbo.getBounds().getWidth(), (int)mParticlesFbo.getBounds().getHeight(), 60);
		cam.setPerspective(60, 1.0f, BUFFER_WIDTH, BUFFER_WIDTH);

		gl::ScopedFramebuffer fboScope(mParticlesFbo.getFbo());
		gl::ScopedViewport viewportScope(ivec2(0), mParticlesFbo.getSize());
		gl::ScopedMatrices matScope;
		gl::setMatrices(cam);

		mParticlesFbo.bindUpdate();
		gl::ScopedTextureBind tex(mParticlesFbo.texture(0), 0);
		gl::ScopedTextureBind tex2(mParticlesFbo.texture(1), 1);

		glBindFragDataLocation(mUpdateShader->getHandle(), 0, "oPosition");
		glBindFragDataLocation(mUpdateShader->getHandle(), 1, "oVelocity");

		mUpdateShader->uniform("positions", 0);
		mUpdateShader->uniform("velocities", 1);

		mParticlesBatch->draw();

		mParticlesFbo.unbindUpdate();
	}
}

void FlockingVisualization::draw()
{
	gl::ScopedTextureBind tex(mParticlesFbo.texture(0), 0);
	mRenderShader->uniform("iPositionTexture", 0);

	mRenderBatch->draw();
}

bool FlockingVisualization::perspective()
{
	return true;
}
