#pragma once

#include "cinder\gl\gl.h"
#include "AudioSource.h"

using namespace ci;

class ShaderVisualization {
public:
	ShaderVisualization();
	void setup(AudioSource audioSource);
	void update();
	void draw();

private:
	AudioSource mAudioSource;
	gl::GlslProgRef mShader;
	gl::TextureRef mTexture;
};
