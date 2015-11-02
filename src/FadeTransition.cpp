#include "FadeTransition.h"

#include "cinder/app/App.h"

using namespace ci;

FadeTransition::FadeTransition(VisualizationRef oldVisualization, VisualizationRef newVisualization, float duration)
{
	mStartTime = app::getElapsedSeconds();
	mDuration = duration;

	mOldVisualization = oldVisualization;
	mNewVisualization = newVisualization;

	gl::GlslProg::Format fadeFmt;
	fadeFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("mix.frag"));
	mMixShader = gl::GlslProg::create(fadeFmt);
}

void FadeTransition::draw(const World & world)
{
	float time = app::getElapsedSeconds();
	float fade = (time - mStartTime) / mDuration;

	fade = glm::clamp<float>(fade, 0, 1);
	mMixShader->uniform("i_add", 1.0f);
	mMixShader->uniform("i_fade", fade);
	mMixShader->uniform("i_resolution", (vec2) world.windowSize);

	gl::ScopedTextureBind A(mNewVisFbo->getColorTexture(), 1);
	mMixShader->uniform("tex_A", 1);

	gl::ScopedTextureBind B(mOldVisFbo->getColorTexture(), 0);
	mMixShader->uniform("tex_B", 0);

	gl::ScopedGlslProg shaderProg(mMixShader);
	gl::context()->setDefaultShaderVars();

	gl::setMatricesWindow(world.windowSize);
	gl::drawSolidRect(world.bounds);
}

void FadeTransition::update(const World & world)
{
	if (mNewVisFbo == nullptr) {
		mNewVisFbo = gl::Fbo::create(world.windowSize.x, world.windowSize.y);
	}

	if (mOldVisFbo == nullptr) {
		mOldVisFbo = gl::Fbo::create(world.windowSize.x, world.windowSize.y);
	}

	mOldVisualization->update(world);
	drawVisualization(world, mOldVisualization, mOldVisFbo);
	mNewVisualization->update(world);
	drawVisualization(world, mNewVisualization, mNewVisFbo);
}

bool FadeTransition::isFinished()
{
	return mStartTime + mDuration < app::getElapsedSeconds();
}

void FadeTransition::drawVisualization(const World& world, VisualizationRef visualization, gl::FboRef fbo)
{
	gl::ScopedFramebuffer sfb(fbo);
	gl::clear(Color(0, 0, 0));
	visualization->draw(world);
}
