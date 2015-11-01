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
	mAlignmentDistance = 6.0;
	mCohesionDistance = 6.0;

	mRoamingDistance = 40.0f;

	mLoudness = 1.0;
	mAccumulatedLoudness = 0.0f;

	mSpeed = 2.0f;

	mBeatConstant = 1.4;

	mHue = 0.0;
	mCycleHueSpeed = 0.0;
	mSaturation = 1.0;

	mSeparateOnly = false;

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

void FlockingVisualization::switchParams(params::InterfaceGlRef params, const string &group) {
	addParamName("Loudness");
	params->addParam("Loudness", &mLoudness)
		.min(0.0)
		.max(2.0)
		.step(0.001)
		.group(group);

	addParamName("Speed");
	params->addParam("Speed", &mSpeed)
		.min(0.5)
		.max(4.0)
		.step(0.001)
		.group(group);

	addParamName(group + "/Beat Constant");
	params->addParam(group + "/Beat Constant", &mBeatConstant)
		.min(1.1)
		.max(2.0)
		.step(0.001)
		.group(group);

	addParamName(group + "/Roaming Distance");
	params->addParam(group + "/Roaming Distance", &mRoamingDistance)
		.min(20.0)
		.max(120.0)
		.step(1.0)
		.group(group);

	addParamName(group + "/Hue");
	params->addParam(group + "/Hue", &mHue)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Saturation");
	params->addParam(group + "/Saturation", &mSaturation)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Cycle Hue Speed");
	params->addParam(group + "/Cycle Hue Speed", &mCycleHueSpeed)
		.min(0.0)
		.max(0.01667)
		.step(0.0001)
		.group(group);

	addParamName(group + "/Separation Distance");
	params->addParam(group + "/Separation Distance", &mSeparationDistance)
		.min(0.0)
		.max(30.0)
		.step(1.0)
		.group(group);

	addParamName(group + "/Cohesion Distance");
	params->addParam(group + "/Cohesion Distance", &mCohesionDistance)
		.min(0.0)
		.max(30.0)
		.step(1.0)
		.group(group);

	addParamName(group + "/Alignment Distance");
	params->addParam(group + "/Alignment Distance", &mAlignmentDistance)
		.min(0.0)
		.max(30.0)
		.step(1.0)
		.group(group);

	addParamName(group + "/Separate Only");
	params->addParam(group + "/Separate Only", &mSeparateOnly);
}


void FlockingVisualization::update(const World& world)
{
	world.audioSource->update();
	world.beatDetector->update(world, mBeatConstant);
	float loudness = audio::linearToDecibel(world.audioSource->getVolume()) * 0.01f * mLoudness;;
	mAccumulatedLoudness += loudness;

	mHue = glm::fract(mHue + mCycleHueSpeed);

	mUpdateShader->uniform("delta", world.deltaSource->delta());
	mUpdateShader->uniform("separationDistance", mSeparationDistance);
	mUpdateShader->uniform("alignmentDistance", mAlignmentDistance);
	mUpdateShader->uniform("cohesionDistance", mCohesionDistance);
	mUpdateShader->uniform("loudness", loudness);
	mUpdateShader->uniform("accumulatedLoudness", mAccumulatedLoudness);
	mUpdateShader->uniform("beat", world.beatDetector->getBeat());
	mUpdateShader->uniform("roamingDistance", mRoamingDistance);
	mUpdateShader->uniform("speed", mSpeed);
	mUpdateShader->uniform("hue", mHue);
	mUpdateShader->uniform("saturation", mSaturation);
	mUpdateShader->uniform("eqs", &(world.audioSource->getEqs(3, mLoudness))[0], 3);
	mUpdateShader->uniform("separateOnly", mSeparateOnly);


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

void FlockingVisualization::draw(const World& world)
{
	world.camera->lookAt(vec3(0.0, 0.0, 100.0), vec3(0.0));

	gl::setMatrices(*world.camera);
	gl::enableDepthRead();
	gl::enableDepthWrite();

	gl::ScopedGlslProg glsl(mRenderShader);
	gl::ScopedVao scopedVao(mVaos[mIteratonIndex & 1]);
	gl::context()->setDefaultShaderVars();

	gl::ScopedState pointSize(GL_PROGRAM_POINT_SIZE, true);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);
}