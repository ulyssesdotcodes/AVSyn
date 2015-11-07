#include "ChoiceVisualization.h"

#include "cinder/app/App.h"

using namespace ci;

ChoiceVisualization::ChoiceVisualization(const World& world, std::map<std::string, std::shared_ptr<Visualization>> visualizations)
{
	mVisualizations = visualizations;
	for (std::map<std::string, std::shared_ptr<Visualization>>::iterator it = mVisualizations.begin(); it != mVisualizations.end(); ++it) {
		mVisualizationNames.push_back(it->first);
	}

	mVisualizationIndex = 0;
	mVisualization = visualizations[mVisualizationNames[mVisualizationIndex]];

	mFadeTransition = nullptr;

	gl::GlslProg::Format shaderFmt;
	shaderFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("feedback.frag"));
	mFeedbackShader = gl::GlslProg::create(shaderFmt);
	mFeedbackShader->uniform("i_resolution", (vec2) world.windowSize);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA16F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);

	mCurrentVis = gl::Fbo::create(world.windowSize.x, world.windowSize.y, fmt);
	mPingPongFBO = PingPongFBO(fmt, world.windowSize, 2);

	mFadeTransitionOn = true;
	mFade = 0.0;
	mScale = 1.0;
	mScaleFade = 0.8;
}

void ChoiceVisualization::update(const World& world)
{
	// If there's a transition, defer to it.
	if (mFadeTransition != nullptr) {
		mFadeTransition->update(world);
	}
	else {
		mVisualization->update(world);
	}

	// If there's a transition, defer to it
	// TODO: Use types
	{
		gl::ScopedFramebuffer fb(mCurrentVis);
		if (mFadeTransition != nullptr) {
			mFadeTransition->draw(world);
			if (mFadeTransition->isFinished()) {
				mFadeTransition = nullptr;
			}
		}
		else {
			gl::clear(Color(0, 0, 0));
			mVisualization->draw(world);
		}
	}
}

void ChoiceVisualization::draw(const World& world)
{

	if (mFade == 0.0 && mScale == 1.0) {
		gl::clear(Color(0, 0, 0));
		gl::setMatricesWindow(world.windowSize);
		gl::draw(mCurrentVis->getColorTexture());
	}
	else {
		{
			gl::ScopedTextureBind prev(mCurrentVis->getColorTexture(), 0);
			mFeedbackShader->uniform("tex_current", 0);

			gl::ScopedTextureBind current(mPingPongFBO.getTexture(), 1);
			mFeedbackShader->uniform("tex_prev", 1);

			mFeedbackShader->uniform("i_fade", mFade);
			mFeedbackShader->uniform("i_scale", mScale);
			mFeedbackShader->uniform("i_scaleFade", mScaleFade);

			gl::setMatricesWindow(world.windowSize);
			mPingPongFBO.render(mFeedbackShader);
		}

		{
			gl::clear(Color(0, 0, 0));
			gl::draw(mPingPongFBO.getTexture());
		}
	}
}

void ChoiceVisualization::switchParams(ci::params::InterfaceGlRef params, const std::string & group)
{
	params->addParam(group + "/Visualization", mVisualizationNames, 
		[=](int ind) {
			VisualizationRef oldVisualization = std::shared_ptr<Visualization>(mVisualization);
			oldVisualization->resetParams(params);

			mVisualizationIndex = ind;
			mVisualization = mVisualizations[mVisualizationNames[mVisualizationIndex]];
			mVisualization->switchParams(params, group + "/Vis");

			if (mFadeTransitionOn) {
				mFadeTransition = std::make_unique<FadeTransition>(oldVisualization, mVisualization, 5.0);
			}
		},
		[=]() {
			return mVisualizationIndex;
		}
	)
		.group(group);

	params->addParam(group + "/Fade Transition", &mFadeTransitionOn)
		.group(group);

	params->addParam(group + "/Feedback/Fade", &mFade)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	params->addParam(group + "/Feedback/Scale", &mScale)
		.min(0.1f)
		.max(2.0f)
		.step(0.01)
		.group(group);

	params->addParam(group + "/Feedback/ScaleFade", &mScaleFade)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	mVisualization->switchParams(params, group);
}
