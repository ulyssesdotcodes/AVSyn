#pragma once

#include "Visualization.h"
#include "AudioShaderVisualization.h"
#include "AudioSource.h"
#include "BeatDetector.h"
#include "cinder\app\App.h"
#include "Kinect2.h"

struct Particle {
	vec3 pos;
	ColorA color;
	vec3 vel;
	vec3 home;
	float damping;
	vec2 ref;
};

class KickChangeImage : public Visualization {
public:
	virtual void setup(AudioSource* audioSource, BeatDetector* beatDetector);
	virtual void update();
	virtual void draw();
	virtual void switchCamera(CameraPersp* camera);
	virtual void switchParams(params::InterfaceGlRef params);
	virtual bool perspective() override;
	virtual void mouseMove(app::MouseEvent mouseEvent);

private:
	AudioSource* mAudioSource;
	BeatDetector* mBeatDetector;
	float mLastBeat;

	AudioShaderVisualization* mVisualization;

	vector<gl::TextureRef> mImages;
	int mCurrentImage;

	vector<Particle> mParticles;

	gl::GlslProgRef mRenderProg;
	gl::GlslProgRef mUpdateProg;

	gl::VaoRef mAttributes[2];
	gl::VboRef mParticleBuffer[2];
	int mIteration;

	gl::FboRef mMousePositionFbo;
	gl::FboRef mVisFbo;

	Kinect2::DeviceRef mDevice;
	Channel8uRef mChannelBodyImage;
};