#pragma once

#include "cinder\gl\gl.h"
#include "AudioSource.h"
#include "Visualization.h"

using namespace ci;

class ShaderVisualization : public Visualization {
public:
	ShaderVisualization();
	void setup(AudioSource audioSource, const fs::path &fragmentShader);
	void update();
	void draw();

private:
	AudioSource mAudioSource;
	gl::GlslProgRef mShader;
	gl::TextureRef mTexture;
};
