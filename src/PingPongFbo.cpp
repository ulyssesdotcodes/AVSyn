#include "PingPongFBO.h"
#include "cinder\gl\Texture.h"

PingPongFBO::PingPongFBO()
{}
 
PingPongFBO::PingPongFBO(vector<Surface32f>& surfaces) : mCurrentFbo(0)
{
	if (surfaces.empty()) return;

	int i = 0;
	mTextureSize = surfaces[0].getSize();
	gl::Fbo::Format format = gl::Fbo::Format();
	format.enableDepthBuffer(false);
	for (Surface32f& s : surfaces) {
		mAttachments.push_back(GL_COLOR_ATTACHMENT0 + i);
		gl::Texture::Format texFmt;
		texFmt.internalFormat(GL_RGBA32F_ARB);
		gl::TextureRef tex = gl::Texture::create(s, texFmt);
		mTextures.push_back(tex);
		format.attachment(GL_COLOR_ATTACHMENT0 + i, tex);

		i++;
	}

	mFbos[0] = gl::Fbo::create(mTextureSize.x, mTextureSize.y, format);
	mFbos[1] = gl::Fbo::create(mTextureSize.x, mTextureSize.y, format);

	gl::ScopedFramebuffer sfb(mFbos[mCurrentFbo]);
	gl::setMatricesWindow(getSize(), false);
	gl::ScopedViewport viewportScope(ivec2(0), getSize());
	gl::ScopedMatrices matScope();
	gl::setMatricesWindow(getSize());
	gl::clear();

	for (int i = 0; i < mAttachments.size(); ++i) {
		gl::drawBuffer(GL_COLOR_ATTACHMENT0 + i);
		gl::ScopedTextureBind tex(mTextures[i]);
		gl::draw(mTextures[i], Rectf(getBounds()));
	}

	mFbos[!mCurrentFbo] = mFbos[mCurrentFbo];
}

void PingPongFBO::addTexture(const Surface32f &surface) {
}
void PingPongFBO::swap()
{
	mCurrentFbo = !mCurrentFbo;
}

void PingPongFBO::bindUpdate()
{
	glDrawBuffers(mAttachments.size(), &mAttachments[0]);

	for (int i = 0; i < mAttachments.size(); ++i) {
		mFbos[!mCurrentFbo]->bindTexture(i, GL_COLOR_ATTACHMENT0 + i);
	}
}

gl::TextureBaseRef PingPongFBO::texture(int ind)
{
	return mTextures[(mCurrentFbo + ind) % 2];
}

void PingPongFBO::unbindUpdate()
{
	swap();
}

vec2 PingPongFBO::getSize() const
{
	return mTextureSize;
}

Area PingPongFBO::getBounds() const
{
	return mFbos[0]->getBounds();
}

gl::FboRef PingPongFBO::getFbo()
{
	return mFbos[mCurrentFbo];
}