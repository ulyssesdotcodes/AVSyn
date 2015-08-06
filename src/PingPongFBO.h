#pragma once

#include "cinder\gl\gl.h"
#include <vector>

using namespace ci;
using namespace std;

class PingPongFBO {
public:
	PingPongFBO();
	PingPongFBO(vector<Surface32f>& surfaces);

	void reset();

	void bindUpdate();
	void unbindUpdate();

	gl::TextureBaseRef texture(int ind);


	gl::FboRef getFbo();

	vec2 getSize() const;
	Area getBounds() const;

private:
	void swap();

	vec2 mTextureSize;
	int mCurrentFbo;

	vector<gl::Texture2dRef> mTextures;
	vector<GLenum> mAttachments;
	gl::FboRef mFbos[2];

	void addTexture(const Surface32f &surface);
};
