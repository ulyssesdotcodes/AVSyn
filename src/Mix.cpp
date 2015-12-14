#include "Mix.h"
#include "cinder\app\App.h"

using namespace ci;

Mix::Mix(const World& world, std::vector<std::string> orderedVisualizationNames, 
	std::map<std::string, std::shared_ptr<Visualization>> visualizations, std::shared_ptr<OscController> oscController)
{
	mChoiceVises[0] = std::make_shared<ChoiceVisualization>(world, orderedVisualizationNames, visualizations,
		OscVisController("/visA", oscController, orderedVisualizationNames));
	mChoiceVises[1] = std::make_shared<ChoiceVisualization>(world, orderedVisualizationNames, visualizations,
		OscVisController("/visB", oscController, orderedVisualizationNames));

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
	//addParamName(group + "/Fade");
	//params->addParam(group + "/Fade", &mFade)
	//	.min(0.0)
	//	.max(1.0)
	//	.step(0.01)
	//	.group(group);

	//addParamName(group + "/Multiply");
	//params->addParam(group + "/Multiply", &mMultiply)
	//	.min(0.0)
	//	.max(6.0)
	//	.step(0.01)
	//	.group(group);

	//addParamName(group + "/Add");
	//params->addParam(group + "/Add", &mAdd)
	//	.min(0.0)
	//	.max(2.0)
	//	.step(0.01)
	//	.group(group);
}

void Mix::update(const World& world)
{
	for (int i = 0; i < 2; ++i) {
		mChoiceVises[i]->update(world);
		gl::ScopedFramebuffer fbo(mVisFBO[i]);
		gl::clear(Color(0, 0, 0));
		gl::ScopedViewport scpVp(mVisFBO[i]->getSize());
		gl::pushMatrices();
		mChoiceVises[i]->draw(world);
		gl::popMatrices();
	}
}

void Mix::draw(const World& world)
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

void Mix::updateUniforms()
{
	mMixShader->uniform("i_fade", mFade);
	mMixShader->uniform("i_add", mAdd);
	mMixShader->uniform("i_multiply", mMultiply);
}
