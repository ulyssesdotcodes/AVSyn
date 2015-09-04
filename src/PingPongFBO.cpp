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

void PingPongFBO::render(gl::GlslProgRef shader)
{
	gl::FboRef target = mFBOs.at((mIteration + 1) % mFBOs.size());
	gl::ScopedFramebuffer fbo(target);
	gl::clear(Color(1, 0, 0));
	gl::ScopedViewport vp(ivec2(0), target->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(target->getSize());
	gl::ScopedGlslProg glsl(shader);

	gl::drawSolidRect(target->getBounds());

	gl::popMatrices();

	// Increment mIteration here so that `getTexture()` always points to the latest
	++mIteration;
}