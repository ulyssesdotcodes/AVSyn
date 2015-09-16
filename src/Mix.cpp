#include "Mix.h"
#include "cinder\app\App.h"

void Mix::setup(map<string, Visualization*> visualizations, vector<string> visualizationOptions)
{
	mVisualizationOptions = visualizationOptions;
	mVisualizations = visualizations;
	mVisOption[0] = 1;
	mVisOption[1] = 0;

	mFade = 0.0;
	mAdd = 1.0;
	mMultiply = 0.0;

	mResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());

	mVisFBO[0] = gl::Fbo::create(mResolution.x, mResolution.y);
	mVisFBO[1] = gl::Fbo::create(mResolution.x, mResolution.y);

	gl::GlslProg::Format shaderProg;
	shaderProg.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("mix.frag"));
	mMixShader = gl::GlslProg::create(shaderProg);
	mMixShader->uniform("i_resolution", mResolution);
}

void Mix::update()
{
	for (int i = 0; i < 2; ++i) {
		Visualization* vis = getVis(mVisOption[i]);
		vis->update();
		gl::ScopedFramebuffer fbo(mVisFBO[i]);
		gl::clear(Color(0, 0, 0));
		gl::ScopedViewport scpVp(mVisFBO[i]->getSize());
		gl::pushMatrices();
		if (vis->perspective()) {
			vis->switchCamera(mCam);
			gl::setMatrices(*mCam);
			switchCamera(mCam);
		}
		else {
			gl::setMatricesWindow(mVisFBO[i]->getSize());
		}
		vis->draw();
		gl::popMatrices();
	}
}

void Mix::draw()
{
	updateUniforms();

	gl::ScopedTextureBind A(mVisFBO[0]->getColorTexture(), 0);
	mMixShader->uniform("tex_A", 0);

	gl::ScopedTextureBind B(mVisFBO[1]->getColorTexture(), 1);
	mMixShader->uniform("tex_B", 1);

	gl::ScopedGlslProg shaderProg(mMixShader);
	gl::context()->setDefaultShaderVars();

	gl::drawSolidRect(app::getWindowIndex(0)->getBounds());
}

bool Mix::perspective()
{
	return false;
}

void Mix::switchCamera(ci::CameraPersp * cam)
{
	mCam = cam;
}

void Mix::switchParams(ci::params::InterfaceGlRef params, const string &group)
{
	mParams = params;

	addParamName(group + "/Fade");
	params->addParam(group + "/Fade", &mFade)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Multiply");
	params->addParam(group + "/Multiply", &mMultiply)
		.min(0.0)
		.max(6.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Add");
	params->addParam(group + "/Add", &mAdd)
		.min(0.0)
		.max(2.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Visualization A");
	params->addParam(group + "/Visualization A", mVisualizationOptions, 
		[=](int ind) {
			getVis(mVisOption[0])->resetParams(mParams);
			mVisOption[0] = ind;
			getVis(mVisOption[0])->switchParams(mParams,"VisA");
		},
			[=]() {
			return mVisOption[0];
		})
		.group(group);

	addParamName(group + "/Visualization B");
	params->addParam(group + "/Visualization B", mVisualizationOptions, 
		[=](int ind) {
			getVis(mVisOption[1])->resetParams(mParams);
			mVisOption[1] = ind;
			getVis(mVisOption[1])->switchParams(mParams, "VisB");
		},
			[=]() {
			return mVisOption[1];
		})
		.group(group);

	getVis(mVisOption[0])->switchParams(mParams, "VisA");
	getVis(mVisOption[1])->switchParams(mParams, "VisB");
}

void Mix::setBaseVisualization(const string & visualization)
{
}

Visualization* Mix::getVis(int index)
{
	return mVisualizations.at(mVisualizationOptions.at(index));
}

void Mix::updateUniforms()
{
	mMixShader->uniform("i_fade", mFade);
	mMixShader->uniform("i_add", mAdd);
	mMixShader->uniform("i_multiply", mMultiply);
}
