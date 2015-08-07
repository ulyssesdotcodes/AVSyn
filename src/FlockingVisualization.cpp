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
	mIteratonIndex = 0;


	vector<string> feedbackVaryings({
		"oPosition",
		"oVelocity"
	});
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("flocking_update.vert"))
		.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings(feedbackVaryings);

	mUpdateShader = gl::GlslProg::create(updateFormat);

	gl::GlslProg::Format renderFormat;
	renderFormat.vertex(app::loadAsset("flocking.vert"))
		.fragment(app::loadAsset("color.frag"))
		.attribLocation("texCoord", 0);
	mRenderShader = gl::GlslProg::create(renderFormat);

	array<vec4, NUM_PARTICLES> positions;
	array<vec3, NUM_PARTICLES> velocities;


	for (int i = 0; i < NUM_PARTICLES; ++i) {
		positions[i] = vec4(SIZE * (Rand::randFloat() - 0.5f),
			SIZE * (Rand::randFloat() - 0.5f),
			SIZE * (Rand::randFloat() - 0.5f), 1.0);

		velocities[i] = Rand::randVec3() * vec3(0.1);
	}

	for (int i = 0; i < 2; ++i) {
		mVaos[i] = gl::Vao::create();
		gl::ScopedVao scopeVao(mVaos[i]);
		{
			mPositions[i] = gl::Vbo::create(GL_ARRAY_BUFFER, positions.size() * sizeof(vec4), positions.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mPositions[i]);
				gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}

			mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mVelocities[i]);
				gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(1);
			}
		}
	}

	mPositionBufTex[0] = gl::BufferTexture::create(mPositions[0], GL_RGBA32F);
	mPositionBufTex[1] = gl::BufferTexture::create(mPositions[1], GL_RGBA32F);
}

void FlockingVisualization::switchCamera(CameraPersp cam) 
{
	cam.lookAt(vec3(0.0, 100.0, 400.0), vec3(0.0));
}

//void FlockingVisualization::setupPingPongFbo() {
//	vector<Surface32f> surfaces;
//	surfaces.push_back(Surface32f(BUFFER_WIDTH, BUFFER_WIDTH, true));
//	Surface32f::Iter pixelIter = surfaces[0].getIter();
//
//	// Initial positions
//	while (pixelIter.line()) {
//		surfaces[0].setPixel(pixelIter.getPos(),
//			ColorAf(SIZE * (Rand::randFloat() - 0.5f),
//				SIZE * (Rand::randFloat() - 0.5f),
//				SIZE * (Rand::randFloat() - 0.5f),
//				1.0));
//	}
//
//	surfaces.push_back(Surface32f(BUFFER_WIDTH, BUFFER_WIDTH, true));
//
//	// Initial velocities
//	while (pixelIter.line()) {
//		surfaces[0].setPixel(pixelIter.getPos(),
//			ColorAf(0.0f, 0.0f, 0.0f, 1.0));
//	}
//
//	mParticlesFbo = PingPongFBO(surfaces);
//}
//
//
//void FlockingVisualization::setupVbo() 
//{
//	vector<vec2> texCoords(NUM_PARTICLES);
//	vector<_Uint32t> indices(NUM_PARTICLES);
//	gl::VboMesh::Layout layout = gl::VboMesh::Layout().usage(GL_STATIC_DRAW).attrib(geom::Attrib::TEX_COORD_0, 2);
//
//	for (int x = 0; x < BUFFER_WIDTH; ++x) {
//		for (int y = 0; y < BUFFER_WIDTH; ++y) {
//			indices.push_back(x * BUFFER_WIDTH + y);
//			texCoords.push_back(vec2(x / (float)BUFFER_WIDTH, y / (float)BUFFER_WIDTH));
//			//texCoords.push_back(vec2(x, y));
//		}
//	}
//
//	auto texCoordsVbo = gl::Vbo::create(GL_ARRAY_BUFFER, texCoords.size() * sizeof(vec2), texCoords.data(), GL_STATIC_DRAW);
//	mVao = gl::Vao::create();
//	gl::ScopedVao vao(mVao);
//	gl::ScopedBuffer buf(texCoordsVbo);
//	gl::enableVertexAttribArray(0);
//	gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), 0);
//}


void FlockingVisualization::update()
{
		gl::ScopedGlslProg glsl(mUpdateShader);
		gl::ScopedState scopeState(GL_RASTERIZER_DISCARD, true);

		gl::ScopedVao scopedVao(mVaos[mIteratonIndex & 1]);
		gl::ScopedTextureBind scopeTex(mPositionBufTex[mIteratonIndex & 1]->getTarget(), mPositionBufTex[mIteratonIndex & 1]->getId());

		mIteratonIndex++;

		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mPositions[mIteratonIndex & 1]);
		gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, mVelocities[mIteratonIndex & 1]);

		gl::beginTransformFeedback(GL_POINTS);
		gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

		gl::endTransformFeedback();


		//CameraPersp cam((int)mParticlesFbo.getBounds().getWidth(), (int)mParticlesFbo.getBounds().getHeight(), 60);
		//cam.setPerspective(60, 1.0f, BUFFER_WIDTH, BUFFER_WIDTH);

		//gl::ScopedFramebuffer fboScope(mParticlesFbo.getFbo());
		//gl::ScopedViewport viewportScope(ivec2(0), mParticlesFbo.getSize());
		//gl::ScopedMatrices matScope;
		//gl::setMatrices(cam);

		//mParticlesFbo.bindUpdate();
		//gl::ScopedTextureBind tex(mParticlesFbo.texture(0), 0);
		//gl::ScopedTextureBind tex2(mParticlesFbo.texture(1), 1);

		//glBindFragDataLocation(mUpdateShader->getHandle(), 0, "oPosition");
		//glBindFragDataLocation(mUpdateShader->getHandle(), 1, "oVelocity");

		//mUpdateShader->uniform("uPositionTexture", 0);
		//mUpdateShader->uniform("uVelocityTexture", 1);

		//mParticlesBatch->draw();

		//mParticlesFbo.unbindUpdate();
}

void FlockingVisualization::draw()
{
	gl::ScopedVao scopedVao(mVaos[mIteratonIndex & 1]);
	gl::ScopedGlslProg glsl(mRenderShader);
	gl::setDefaultShaderVars();

	gl::pointSize(4.0f);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
	//gl::ScopedTextureBind tex(mParticlesFbo.texture(0), 0);
	//mRenderShader->uniform("uPositionTexture", 0);

	//gl::ScopedGlslProg render(mRenderShader);
	//gl::ScopedVao vao(mVao);
	//gl::context()->setDefaultShaderVars();
	//gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

bool FlockingVisualization::perspective()
{
	return true;
}
