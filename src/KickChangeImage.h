#pragma once

#include "Visualization.h"
#include "AudioSource.h"

class KickChangeImage : public Visualization {
public:
	virtual void setup(AudioSource* audioSource);
	virtual void update();
	virtual void draw();
	virtual void switchCamera(CameraPersp camera);
	virtual void switchParams(params::InterfaceGlRef params);
	virtual bool perspective() override;

private:
	AudioSource* mAudioSource;

	gl::TextureRef mCurrentImage;

};