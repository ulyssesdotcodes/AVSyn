#include "FlockingVisualization.h"

#include "cinder\app\App.h"
#include "cinder\Rand.h"
#include "cinder\Camera.h"

using namespace ci;

const int SIZE = 50;
const int BUFFER_WIDTH = 128;
const int NUM_PARTICLES = BUFFER_WIDTH * BUFFER_WIDTH;

FlockingVisualization::FlockingVisualization()
{
	mSeparationDistance = 6.0;
	mAlignmentDistance = 12.0;
	mCohesionDistance = 6.0;

	mRoamingDistance = 40.0f;

	mAccumulatedLoudness = 0.0f;

	mSpeed = 3.0f;
}

void FlockingVisualization::setup(AudioSource* audioSource, DeltaSource* deltaSource, BeatDetector* beatDetector)
{
	mAudioSource = audioSource;
	mDeltaSource = deltaSource;
	mBeatDetector = beatDetector;

	mStep = true;
	mIteratonIndex = 0;


	vector<string> feedbackVaryings({
		"tf_position",
		"tf_velocity",
		"tf_color"
	});
	gl::GlslProg::Format updateFormat;
	updateFormat.vertex(app::loadAsset("flocking_update.vert"))
		.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings(feedbackVaryings);

	mUpdateShader = gl::GlslProg::create(updateFormat);

	gl::GlslProg::Format renderFormat;
	renderFormat.vertex(app::loadAsset("flocking.vert"))
		.fragment(app::loadAsset("point_circle.frag"));

	mRenderShader = gl::GlslProg::create(renderFormat);

	array<vec3, NUM_PARTICLES> positions;
	array<vec3, NUM_PARTICLES> velocities;
	array<vec3, NUM_PARTICLES> colors;


	for (int i = 0; i < NUM_PARTICLES; ++i) {
		positions[i] = vec3(SIZE * (Rand::randFloat() - 0.5f),
			SIZE * (Rand::randFloat() - 0.5f),
			SIZE * (Rand::randFloat() - 0.5f));

		velocities[i] = Rand::randVec3();
		colors[i] = vec3(0, 0, 0);
	}

	for (int i = 0; i < 2; ++i) {
		mVaos[i] = gl::Vao::create();
		gl::ScopedVao scopeVao(mVaos[i]);
		{
			mPositions[i] = gl::Vbo::create(GL_ARRAY_BUFFER, positions.size() * sizeof(vec3), positions.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mPositions[i]);
				gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}

			mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mVelocities[i]);
				gl::vertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(1);
			}

			mColors[i] = gl::Vbo::create(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), colors.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mColors[i]);
				gl::vertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(2);
			}
		}
	}

	mPositionBufTex[0] = gl::BufferTexture::create(mPositions[0], GL_RGB32F);
	mPositionBufTex[1] = gl::BufferTexture::create(mPositions[1], GL_RGB32F);
	mVelocityBufTex[0] = gl::BufferTexture::create(mVelocities[0], GL_RGB32F);
	mVelocityBufTex[1] = gl::BufferTexture::create(mVelocities[1], GL_RGB32F);
}

void FlockingVisualization::switchCamera(CameraPersp cam) 
{
	cam.lookAt(vec3(0.0, 100.0, 400.0), vec3(0.0));
}


void FlockingVisualization::update()
{
	mAudioSource->update();
	mBeatDetector->update(1.4);
	float loudness = audio::linearToDecibel(mAudioSource->getVolume()) * 0.01f;
	mAccumulatedLoudness += loudness;

	mUpdateShader->uniform("delta", mDeltaSource->delta());
	mUpdateShader->uniform("separationDistance", mSeparationDistance);
	mUpdateShader->uniform("alignmentDistance", mSeparationDistance);
	mUpdateShader->uniform("cohesionDistance", mSeparationDistance);
	mUpdateShader->uniform("loudness", loudness);
	mUpdateShader->uniform("accumulatedLoudness", mAccumulatedLoudness);
	mUpdateShader->uniform("beat", mBeatDetector->getBeat());
	mUpdateShader->uniform("roamingDistance", mRoamingDistance);
	mUpdateShader->uniform("speed", mSpeed);
	mUpdateShader->uniform("eqs", &(mAudioSource->getEqs(3))[0], 3);


	gl::ScopedVao scopedVao(mVaos[mIteratonIndex & 1]);
	gl::ScopedTextureBind scopeTexPos(mPositionBufTex[mIteratonIndex & 1]->getTarget(), mPositionBufTex[mIteratonIndex & 1]->getId(), 0);
	gl::ScopedTextureBind scopeTexVel(mVelocityBufTex[mIteratonIndex & 1]->getTarget(), mVelocityBufTex[mIteratonIndex & 1]->getId(), 1);

	mUpdateShader->uniform("tex_position", 0);
	mUpdateShader->uniform("tex_velocity", 1);

	gl::ScopedGlslProg glsl(mUpdateShader);
	gl::ScopedState scopeState(GL_RASTERIZER_DISCARD, true);

	mIteratonIndex++;

	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, mPositions[mIteratonIndex & 1]);
	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 1, mVelocities[mIteratonIndex & 1]);
	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 2, mColors[mIteratonIndex & 1]);

	gl::beginTransformFeedback(GL_POINTS);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::endTransformFeedback();
}

void FlockingVisualization::draw()
{
	gl::ScopedTextureBind scopeTexPos(mPositionBufTex[mIteratonIndex & 1]->getTarget(), mPositionBufTex[mIteratonIndex & 1]->getId(), 0);
	mRenderShader->uniform("tex_position", 0);

	gl::ScopedGlslProg glsl(mRenderShader);
	gl::ScopedVao scopedVao(mVaos[mIteratonIndex & 1]);
	gl::context()->setDefaultShaderVars();

	gl::ScopedState pointSize(GL_PROGRAM_POINT_SIZE, true);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}

bool FlockingVisualization::perspective()
{
	return true;
}
