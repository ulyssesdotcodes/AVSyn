#pragma once

#include "cinder\gl\gl.h"
#include <vector>

using namespace std;
using namespace ci;

class PingPongFBO {
public:
	PingPongFBO();
	PingPongFBO(gl::Fbo::Format fmt, vec2 size, int buffers);
	gl::TextureRef getTexture();
	vector<gl::TextureRef> getTextures();
	void render(gl::GlslProgRef shader);

private:
	vector<gl::FboRef> mFBOs;
	int mIteration;
};
