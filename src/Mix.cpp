#include "Mix.h"
#include "cinder\app\App.h"

using namespace ci;

Mix::Mix(const World& world, std::vector<std::string> orderedVisualizationNames, 
	std::map<std::string, std::shared_ptr<Visualization>> visualizations, std::shared_ptr<OscController> oscController) : 
	mOscController(oscController)
{
	mChoiceVises[0] = std::make_shared<ChoiceVisualization>(world, orderedVisualizationNames, visualizations,
		OscVisController("/visA", oscController, orderedVisualizationNames));
	mChoiceVises[1] = std::make_shared<ChoiceVisualization>(world, orderedVisualizationNames, visualizations,
		OscVisController("/visB", oscController, orderedVisualizationNames));

	mFade = 0.5;

	mVisFBO[0] = gl::Fbo::create(world.windowSize.x, world.windowSize.y);
	mVisFBO[1] = gl::Fbo::create(world.windowSize.x, world.windowSize.y);

	gl::GlslProg::Format shaderProg;
	shaderProg.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("mix.frag"));
	mMixShader = gl::GlslProg::create(shaderProg);
	mMixShader->uniform("i_resolution", (vec2) world.windowSize);
	mMixShader->uniform("i_fade", mFade);


	osc::Message choicesMessage;
	choicesMessage.setAddress("/choices");

	for (std::vector<std::string>::const_iterator it = orderedVisualizationNames.begin(); it != orderedVisualizationNames.end(); ++it) {
		choicesMessage.addStringArg(*it);
	}

	mOscController.subscribeToConnection([this, oscController, choicesMessage](osc::Message __) {
		oscController->sendMessage(choicesMessage);
		this->onConnection();
	});
}

void Mix::update(const World& world)
{
	if (mFade < 1) {
		mChoiceVises[0]->update(world);
	}

	if (mFade > 0) {
		mChoiceVises[1]->update(world);
	}
}

void Mix::draw(const World& world)
{
	if (mFade == 0) {
		mChoiceVises[0]->draw(world);
	}
	else if (mFade == 1) {
		mChoiceVises[1]->draw(world);
	}
	else {
		for (int i = 0; i < 2; ++i) {
			gl::ScopedFramebuffer fbo(mVisFBO[i]);
			gl::clear(Color(0, 0, 0));
			gl::ScopedViewport scpVp(mVisFBO[i]->getSize());
			gl::pushMatrices();
			mChoiceVises[i]->draw(world);
			gl::popMatrices();
		}

		{
			gl::clear(Color(0, 0, 0));

			gl::ScopedTextureBind A(mVisFBO[0]->getColorTexture(), 0);
			mMixShader->uniform("tex_A", 0);

			gl::ScopedTextureBind B(mVisFBO[1]->getColorTexture(), 1);
			mMixShader->uniform("tex_B", 1);

			gl::ScopedGlslProg shaderProg(mMixShader);
			gl::context()->setDefaultShaderVars();

			gl::ScopedViewport vp(ivec2(0), world.windowSize);
			gl::pushMatrices();
			gl::setMatricesWindow(world.windowSize);

			gl::drawSolidRect(world.bounds);

			gl::popMatrices();
		}
	}
}

void Mix::onConnection()
{
	for (int i = 0; i < 2; ++i) {
		mChoiceVises[i]->onConnection();
	}

	mOscController.subscribeSliderListener("Fade", 0, 1, 0.5, [&](auto val) { 
		mFade = val; 
		mMixShader->uniform("i_fade", mFade);
	});

	mOscController.subscribeSliderListener("Add", 0, 2, 1, mMixShader, "i_add");
	mOscController.subscribeSliderListener("Multiply", 0, 6, 0, mMixShader, "i_multiply");
}