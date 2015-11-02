#pragma once

#include "cinder/gl/gl.h"

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

	ci::gl::GlslProgRef mMixShader;
	ci::gl::FboRef mOldVisFbo,
	 mNewVisFbo;

	void drawVisualization(const World& world, VisualizationRef visualization, ci::gl::FboRef fbo);
};