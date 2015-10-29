#include "PingPongFBO.h"

PingPongFBO::PingPongFBO()
{
}

PingPongFBO::PingPongFBO(gl::Fbo::Format fmt, vec2 size, int buffers)
{
	mIteration = 0;
	for (int i = 0; i < buffers; ++i) {
		gl::FboRef fboRef = gl::Fbo::create(size.x, size.y, fmt);
		gl::ScopedFramebuffer fbo(fboRef);
		gl::clear(Color(0, 0, 0));
		mFBOs.push_back(fboRef);
	}
}

gl::TextureRef PingPongFBO::getTexture()
{
	return mFBOs.at(mIteration % mFBOs.size())->getColorTexture();
}

vector<gl::TextureRef> PingPongFBO::getTextures()
{
	vector<gl::TextureRef> textures;

	for (int i = mIteration - mFBOs.size() + 1; i <= mIteration; ++i) {
		textures.push_back(mFBOs.at(i % mFBOs.size())->getColorTexture());
	}

	return textures;
}

void PingPongFBO::render(gl::GlslProgRef shader)
{
	gl::FboRef target = mFBOs.at((mIteration + 1) % mFBOs.size());
	gl::ScopedFramebuffer fbo(target);
	gl::clear(Color(0, 0, 0));
	gl::ScopedViewport vp(ivec2(0), target->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(target->getSize());
	gl::ScopedGlslProg glsl(shader);

	gl::drawSolidRect(target->getBounds());

	gl::popMatrices();

	// Increment mIteration here so that `getTexture()` always points to the latest
	++mIteration;
}

void PingPongFBO::render(gl::BatchRef batch, vec2 translate, Colorf color)
{
	gl::FboRef target = mFBOs.at((mIteration + 1) % mFBOs.size());
	gl::ScopedFramebuffer fbo(target);

	gl::ScopedViewport vp(ivec2(0), target->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(target->getSize());

	gl::color(color);
	gl::translate(translate);

	batch->draw();

	// Increment mIteration here so that `getTexture()` always points to the latest
	++mIteration;
}
