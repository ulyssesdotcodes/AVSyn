#pragma once

#include "cinder/gl/gl.h"

class PingPongFBO {
public:
	PingPongFBO();
	PingPongFBO(ci::gl::Fbo::Format fmt, ci::vec2 size, int buffers);
	ci::gl::TextureRef getTexture();
	std::vector<ci::gl::TextureRef> getTextures();
	void render(ci::gl::GlslProgRef shader);
	void render(ci::gl::BatchRef batch, ci::vec2 translate = ci::vec2(0), ci::Colorf color = ci::Colorf::white());
	ci::Rectf getBounds();

private:
	std::vector<ci::gl::FboRef> mFBOs;
	int mIteration;
};
