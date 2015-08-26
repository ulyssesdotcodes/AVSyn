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
	updateFormat.vertex(app::loadAsset("apply_force.vert"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	updateFormat.vertex(app::loadAsset("velocity_divergence.vert"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	updateFormat.vertex(app::loadAsset("solve_pressure.vert"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);
	updateFormat.vertex(app::loadAsset("subtract_pressure.vert"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);

	array<vec3, NUM_PARTICLES> velocities;
	array<vec2, NUM_PARTICLES> pressures;
	array<ivec4, NUM_PARTICLES> connections;

	velocities.assign(vec3(0));
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
			mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec3), velocities.data(), GL_DYNAMIC_DRAW);
			{
				gl::ScopedBuffer scopeBuffer(mVelocities[i]);
				gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
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

		mVelocityBufTexs[i] = gl::BufferTexture::create(mVelocities[i], GL_RGB);
		mPressureBufTexs[i] = gl::BufferTexture::create(mPressures[i], GL_FLOAT);
	}

}

void Fluid::update()
{
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

void Fluid::advect()
{
}

void Fluid::applyForce()
{
}

void Fluid::computeDivergence()
{
}

void Fluid::solvePressure()
{
}

void Fluid::subtractPressure()
{
}
