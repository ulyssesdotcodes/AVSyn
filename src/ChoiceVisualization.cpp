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
	mOffsetY = 0.0;
	mHueShift = 0.0;
	mHueShiftCycle = 0.0;
	mSaturationShift = 0.0;
	mLightnessShift = 1.0;
	mManipFade = 0.0;
}

void ChoiceVisualization::update(const World& world)
{
	mHueShift = glm::fract(mHueShift + mHueShiftCycle * world.deltaSource->delta());

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

	if (mFade == 0.0 && mScale == 1.0 && mHueShift == 0.0 && mSaturationShift == 0 && mLightnessShift == 0) {
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
			mFeedbackShader->uniform("i_offsetY", mOffsetY);
			mFeedbackShader->uniform("i_hueShift", mHueShift);
			mFeedbackShader->uniform("i_manipFade", mManipFade);
			mFeedbackShader->uniform("i_saturationShift", mSaturationShift);
			mFeedbackShader->uniform("i_lightnessShift", mLightnessShift);

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
	addParamName(group + "/Visualization");
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

	addParamName(group + "/Fade Transition");
	params->addParam(group + "/Fade Transition", &mFadeTransitionOn)
		.group(group);

	addParamName(group + "/Feedback/Fade");
	params->addParam(group + "/Feedback/Fade", &mFade)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/Scale");
	params->addParam(group + "/Feedback/Scale", &mScale)
		.min(0.1f)
		.max(2.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/OffsetY");
	params->addParam(group + "/Feedback/OffsetY", &mOffsetY)
		.min(-1.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/ManipFade");
	params->addParam(group + "/Feedback/ManipFade", &mManipFade)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/Reset");
	params->addButton(group + "/Feedback/Reset",
		[=]() {
			mFade = 0.98;
			mScale = 1.0;
			mManipFade = 0.0;
		}, "group=" + group);

	addParamName(group + "/Feedback/HueShift");
	params->addParam(group + "/Feedback/HueShift", &mHueShift)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/SaturationShift");
	params->addParam(group + "/Feedback/SaturationShift", &mSaturationShift)
		.min(0.0f)
		.max(1.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/LightnessShift");
	params->addParam(group + "/Feedback/LightnessShift", &mLightnessShift)
		.min(0.0f)
		.max(2.0f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/HueShiftCycle");
	params->addParam(group + "/Feedback/HueShiftCycle", &mHueShiftCycle)
		.min(0.0f)
		.max(1.00f)
		.step(0.01)
		.group(group);

	addParamName(group + "/Feedback/ResetColor");
	params->addButton(group + "/Feedback/ResetColor",
		[=]() {
			mHueShift = 0.0;
			mHueShiftCycle = 0.0;
			mSaturationShift = 0.0;
			mLightnessShift = 1.0;
		}, "group=" + group);

	mVisualization->switchParams(params, group);
}
