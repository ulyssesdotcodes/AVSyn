#pragma once

#include "cinder\gl\gl.h"

#include "PingPongFBO.h"
#include "Visualization.h"

class FadeTransition {
public:
	FadeTransition(VisualizationRef oldVisualization, VisualizationRef newVisualization, float duration);
	void draw(const World& world);
	void update(const World& world);
	bool isFinished();

private:
	float mStartTime;
	float mDuration;

	VisualizationRef mOldVisualization,
		mNewVisualization;

	gl::GlslProgRef mMixShader;
	gl::FboRef mOldVisFbo,
	 mNewVisFbo;

	void drawVisualization(const World& world, VisualizationRef visualization, gl::FboRef fbo);
};