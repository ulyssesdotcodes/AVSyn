#include "Fluid.h"
#include "cinder\app\App.h"
#include "cinder\Rand.h"

const int BUFFER_WIDTH = 100;
const int BUFFER_HEIGHT = 100;
const int NUM_PARTICLES = BUFFER_WIDTH * BUFFER_HEIGHT;

void Fluid::setup()
{
	// Setup the shaders
	vector<string> targetVaryings({ "tf_target" });
	vector<string> velocityVaryings({ "tf_velocity" });
	vector<string> pressureVaryings({ "tf_pressure" });
	gl::GlslProg::Format updateFormat;
	///////////
	// Target Varying
	updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings(targetVaryings);

	updateFormat.vertex(app::loadAsset("Fluid/advect.vert"));
	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("WIDTH", BUFFER_WIDTH);
	mAdvectShader->uniform("HEIGHT", BUFFER_HEIGHT);

	updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings(velocityVaryings);
	updateFormat.vertex(app::loadAsset("Fluid/apply_force.vert"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("WIDTH", BUFFER_WIDTH);
	mForcesShader->uniform("HEIGHT", BUFFER_HEIGHT);

	updateFormat.vertex(app::loadAsset("Fluid/subtract_pressure.vert"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);
	mSubtractPressureShader->uniform("WIDTH", BUFFER_WIDTH);
	mSubtractPressureShader->uniform("HEIGHT", BUFFER_HEIGHT);

	updateFormat = gl::GlslProg::Format();
	updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
		.feedbackVaryings(pressureVaryings);
	updateFormat.vertex(app::loadAsset("Fluid/velocity_divergence.vert"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	updateFormat.vertex(app::loadAsset("Fluid/solve_pressure.vert"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);

	gl::GlslProg::Format renderFormat;
	renderFormat
		.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("Fluid/render.frag"));
	mRenderShader = gl::GlslProg::create(renderFormat);
	mRenderShader->uniform("WIDTH", BUFFER_WIDTH);
	mRenderShader->uniform("HEIGHT", BUFFER_HEIGHT);

	// Even though we only use 2 of these, every advected quantity has to be a vec4
	array<vec4, NUM_PARTICLES> velocities;
	array<vec2, NUM_PARTICLES> pressures;
	array<ivec4, NUM_PARTICLES> connections;
	array<vec4, NUM_PARTICLES> dye;

	velocities.assign(vec4(0));
	pressures.assign(vec2(0));

	int n = 0;
	for (int j = 0; j < BUFFER_HEIGHT; ++j) {
		for (int i = 0; i < BUFFER_WIDTH; ++i) {
			// left, top, right, bottom in that order
			connections[n][0] = i > 0 ? n - 1 : -1;
			connections[n][1] = j > 0 ? n - BUFFER_WIDTH : -1;
			connections[n][2] = i < BUFFER_WIDTH - 1 ? n + 1 : -1;
			connections[n][3] = j < BUFFER_HEIGHT - 1 ? n + BUFFER_WIDTH : -1;

			//velocities[n] = glm::vec4(Rand::randVec3(), 1.0);
			dye[n] = vec4((float)i / (float)BUFFER_WIDTH, (float)j / (float)BUFFER_HEIGHT, 0, 1);

			n++;
		}
	}


	for (int i = 0; i < 2; ++i) {
		mVaos[i] = gl::Vao::create();
		{
			gl::ScopedVao scopeVao(mVaos[i]);
			mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec4), velocities.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mVelocities[i]);
				gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}

			mPressures[i] = gl::Vbo::create(GL_ARRAY_BUFFER, pressures.size() * sizeof(vec2), pressures.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mPressures[i]);
				gl::vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(1);
			}

			mConnections[i] = gl::Vbo::create(GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mConnections[i]);
				gl::vertexAttribIPointer(2, 4, GL_INT, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(2);
			}
		}

		mDyes[i] = gl::Vbo::create(GL_ARRAY_BUFFER, dye.size() * sizeof(vec4), dye.data(), GL_STATIC_DRAW);

		mPressureVaos[i] = gl::Vao::create();
		{
			gl::ScopedVao scopedVao(mPressureVaos[i]);
			{
				gl::ScopedBuffer scopeBuffer(mPressures[i]);
				gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(0);
			}
			{
				gl::ScopedBuffer scopeBuffer(mConnections[i]);
				gl::vertexAttribIPointer(1, 4, GL_INT, 0, (const GLvoid*)0);
				gl::enableVertexAttribArray(1);
			}
		}

		mVelocityBufTexs[i] = gl::BufferTexture::create(mVelocities[i], GL_RGBA32F);
		mPressureBufTexs[i] = gl::BufferTexture::create(mPressures[i], GL_RG32F);
		mDyeBufTexs[i] = gl::BufferTexture::create(mDyes[i], GL_RG32F);
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

	advectDye(dt);
}

void Fluid::mouseDrag(app::MouseEvent mouseEvent) 
{
	mForcesShader->uniform("isMouseDown", mouseEvent.isLeftDown());
	mForcesShader->uniform("mouse", mouseEvent.getPos());
	mForcesShader->uniform("lastMouse", mLastMouse);

	mLastMouse = mouseEvent.getPos();
}

void Fluid::draw()
{
	mRenderShader->uniform("resolution", vec3(app::getWindowWidth(), app::getWindowHeight(), 0.0f));
	gl::ScopedTextureBind scopeVel(mDyeBufTexs[mDyeIteration & 1]->getTarget(), mDyeBufTexs[mDyeIteration & 1]->getId(), 2);
	mRenderShader->uniform("tex_dye", 2);
	gl::ScopedGlslProg glsl(mRenderShader);
	//gl::ScopedVao scopedVao(mVaos[mIteration & 1]);
	gl::context()->setDefaultShaderVars();

	gl::drawSolidRect(app::getWindowBounds());
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
	++mIteration;
}

void Fluid::advectDye(float dt) 
{
	mAdvectShader->uniform("dt", dt);
	gl::ScopedTextureBind scopeVel(mDyeBufTexs[mDyeIteration & 1]->getTarget(), mDyeBufTexs[mDyeIteration & 1]->getId(), 2);
	mAdvectShader->uniform("tex_target", 2);

	renderToBuffer(mAdvectShader, mVaos[mIteration & 1], mDyes[(mDyeIteration + 1) & 1]);
	++mDyeIteration;
}

void Fluid::applyForce(float dt)
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);
	mDivergenceShader->uniform("tex_velocity", 0);
	mForcesShader->uniform("dt", dt);
	mForcesShader->uniform("time", mLastTime);
	renderToBuffer(mForcesShader, mVaos[mIteration & 1], mVelocities[(mIteration + 1) & 1]);
	++mIteration;

	mForcesShader->uniform("isMouseDown", false);
}

void Fluid::computeDivergence()
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getTarget(), mVelocityBufTexs[mIteration & 1]->getId(), 0);
	mDivergenceShader->uniform("tex_velocity", 0);

	renderToBuffer(mDivergenceShader, mPressureVaos[mPressureIteration & 1], mPressures[(mPressureIteration + 1) & 1]);
	++mPressureIteration;
}

void Fluid::solvePressure()
{
	int i;
	for (i = mPressureIteration; i < mPressureIteration + 30; ++i) {
		gl::ScopedTextureBind scopeVel(mPressureBufTexs[i & 1]->getTarget(), mPressureBufTexs[i & 1]->getId(), 1);
		mPressureSolveShader->uniform("tex_pressure", 1);
		renderToBuffer(mPressureSolveShader, mPressureVaos[i & 1], mPressures[(i + 1) & 1]);
	}
	mPressureIteration = i;
}

void Fluid::subtractPressure()
{
	gl::ScopedTextureBind scopePressure(mPressureBufTexs[mPressureIteration & 1]->getTarget(), 
		mPressureBufTexs[mPressureIteration & 1]->getId(), 1);
	mSubtractPressureShader->uniform("tex_pressure", 1);

	renderToBuffer(mSubtractPressureShader, mVaos[mIteration & 1], mVelocities[(mIteration + 1) & 1]);
	++mIteration;
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
