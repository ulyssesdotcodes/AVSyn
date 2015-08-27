#include "Fluid.h"
#include "cinder\app\App.h"

const int BUFFER_WIDTH = 512;
const int BUFFER_HEIGHT = 512;
const int NUM_PARTICLES = BUFFER_WIDTH * BUFFER_HEIGHT;

void Fluid::setup()
{
	// Setup the shaders
	vector<string> feedbackVaryings({ "tf_velocity" });
	gl::GlslProg::Format updateFormat;
	updateFormat.feedbackVaryings(feedbackVaryings)
		.feedbackFormat(GL_SEPARATE_ATTRIBS);

	updateFormat.vertex(app::loadAsset("advect.vert"));
	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("WIDTH", BUFFER_WIDTH);

	updateFormat.vertex(app::loadAsset("apply_force.vert"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("WIDTH", BUFFER_WIDTH);

	updateFormat.vertex(app::loadAsset("subtract_pressure.vert"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);

	vector<string> pressureVaryings({ "tf_pressure" });
	gl::GlslProg::Format updateFormat;
	updateFormat.feedbackVaryings(feedbackVaryings)
		.feedbackFormat(GL_SEPARATE_ATTRIBS);
	updateFormat.vertex(app::loadAsset("velocity_divergence.vert"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	updateFormat.vertex(app::loadAsset("solve_pressure.vert"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);

	// Even though we only use 2 of these, every advected quantity has to be a vec4
	array<vec4, NUM_PARTICLES> velocities;
	array<vec2, NUM_PARTICLES> pressures;
	array<ivec4, NUM_PARTICLES> connections;

	velocities.assign(vec4(0));
	pressures.assign(vec2(0));

	int n = 0;
	for (int j = 0; j < BUFFER_HEIGHT; ++j) {
		for (int i = 0; i < BUFFER_WIDTH; ++i) {
			// left, above, right, below in that order
			connections[n][0] = i > 0 ? n - 1 : -1;
			connections[n][1] = j > 0 ? n - BUFFER_WIDTH : -1;
			connections[n][2] = i < BUFFER_WIDTH - 1 ? n + 1 : -1;
			connections[n][3] = j < BUFFER_HEIGHT - 1 ? n + BUFFER_WIDTH : -1;
			n++;
		}
	}


	for (int i = 0; i < 2; ++i) {
		mVaos[i] = gl::Vao::create();
		{
			gl::ScopedVao scopeVao(mVaos[i]);
			mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec4), velocities.data(), GL_DYNAMIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mVelocities[i]);
				gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}

			mPressures[i] = gl::Vbo::create(GL_ARRAY_BUFFER, pressures.size() * sizeof(vec2), pressures.data(), GL_DYNAMIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mPressures[i]);
				gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(1);
			}

			mConnections[i] = gl::Vbo::create(GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mPressures[i]);
				gl::vertexAttribIPointer(0, 1, GL_INT, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(2);
			}
		}

		mPressureVaos[i] = gl::Vao::create();
		{
			gl::ScopedVao scopedVao(mPressureVaos[i]);
			{
				gl::ScopedBuffer scopeBuffer(mPressures[i]);
				gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}
		}

		mVelocityBufTexs[i] = gl::BufferTexture::create(mVelocities[i], GL_RG);
		mPressureBufTexs[i] = gl::BufferTexture::create(mPressures[i], GL_RG);
	}

}

void Fluid::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	advect(dt);

	applyForce(dt);

	computeDivergence();
	solvePressure();
	subtractPressure();
}

void Fluid::draw()
{

}

bool Fluid::perspective()
{
	return false;
}

void Fluid::switchCamera(CameraPersp * camera)
{
}

void Fluid::switchParams(params::InterfaceGlRef params)
{
}

void Fluid::advect(float dt)
{
	mAdvectShader->uniform("dt", dt);
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);
	mAdvectShader->uniform("tex_target", 0);

	renderToBuffer(mAdvectShader, mVaos[mIteration & 1], mVelocities[(mIteration + 1) & 1]);
}

void Fluid::applyForce(float dt)
{
	mForcesShader->uniform("dt", dt);
	renderToBuffer(mForcesShader, mVaos[mIteration & 1], mVelocities[(mIteration + 1) & 1]);
	++mIteration;
}

void Fluid::computeDivergence()
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);
	mDivergenceShader->uniform("tex_velocity", 0);

	renderToBuffer(mDivergenceShader, mVaos[mIteration & 1], mPressures[(mIteration + 1) & 1]);
	++mIteration;
}

void Fluid::solvePressure()
{
	int i;
	for (i = mIteration; i < mIteration + 30; ++i) {
		gl::ScopedTextureBind scopeVel(mPressureBufTexs[i & 1]->getTarget(), mPressureBufTexs[i & 1]->getId(), 0);
		mPressureSolveShader->uniform("tex_pressure", 0);
		renderToBuffer(mPressureSolveShader, mPressureVaos[i & 1], mPressures[(i + 1) & 1]);
	}

	if (mIteration & 1 != i & 1) {
		gl::ScopedTextureBind scopeVel(mPressureBufTexs[i & 1]->getTarget(), mPressureBufTexs[i & 1]->getId(), 0);
		mPressureSolveShader->uniform("tex_pressure", 0);
		renderToBuffer(mPressureSolveShader, mPressureVaos[i & 1], mPressures[mIteration & 1]);
	}
}

void Fluid::subtractPressure()
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);
	gl::ScopedTextureBind scopePressure(mPressureBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);

	renderToBuffer(mSubtractPressureShader, mVaos[mIteration & 1], mVelocities[(mIteration + 1) & 1]);
}

void Fluid::renderToBuffer(gl::GlslProgRef shader, gl::VaoRef vao, gl::VboRef target)
{
	gl::ScopedVao scopevao(vao);
	gl::ScopedGlslProg glsl(shader);
	gl::ScopedState scopeState(GL_RASTERIZER_DISCARD, true);

	gl::bindBufferBase(GL_TRANSFORM_FEEDBACK_BUFFER, 0, target);

	gl::beginTransformFeedback(GL_POINTS);
	gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::endTransformFeedback();
}
