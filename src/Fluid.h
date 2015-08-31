#pragma once

#include "Visualization.h"
#include "cinder\gl\gl.h"
#include "cinder\gl\BufferTexture.h"
#include "cinder\app\App.h"

using namespace ci;

class Fluid : public Visualization {
public:
	virtual void setup();
	virtual void update() override;
	virtual void draw() override;
	virtual bool perspective() override;
	virtual void switchCamera(CameraPersp* camera) override;
	virtual void switchParams(params::InterfaceGlRef params) override;
	virtual void mouseDrag(app::MouseEvent mouseEvent);
	virtual void mouseUp(app::MouseEvent mouseEvent);

private:
	gl::GlslProgRef mAdvectShader, mForcesShader, mDivergenceShader, mPressureSolveShader, mSubtractPressureShader, mRenderShader;

	//array<gl::VaoRef, 2> mVaos, mPressureVaos;
	//array<gl::VboRef, 2> mVelocities, mPressures, mConnections, mDyes;
	array<gl::FboRef, 2> mVelocityBufTexs, mPressureBufTexs, mDyeBufTexs;

	void advect(float dt);
	void advectDye(float dt);
	void applyForce(float dt);
	void computeDivergence();
	void solvePressure();
	void subtractPressure();
	void renderToBuffer(gl::GlslProgRef shader, gl::FboRef target);

	int mIteration;
	int mPressureIteration;
	int mDyeIteration;

	vec2 mWindowResolution;
	vec2 mFluidResolution;
	float mLastTime;
	vec2 mLastMouse;
};