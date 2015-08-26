#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\BufferTexture.h"

using namespace ci;

class Fluid : public Visualization {
public:
	virtual void setup();
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp* camera) override;
	virtual void switchParams(params::InterfaceGlRef params) override;

private:
	gl::GlslProgRef mAdvectShader, mForcesShader, mDivergenceShader, mPressureSolveShader, mSubtractPressureShader, mRenderShader;

	array<gl::VaoRef, 2> mVaos, mPressureVaos;
	array<gl::VboRef, 2> mVelocities, mPressures, mConnections;
	array<gl::BufferTextureRef, 2> mVelocityBufTexs, mPressureBufTexs;

	void advect(float dt);
	void applyForce(float dt);
	void computeDivergence();
	void solvePressure();
	void subtractPressure();
	void renderToBuffer(gl::GlslProgRef shader, gl::VaoRef vao, gl::VboRef target);

	int mIteration;

	float mLastTime;
};