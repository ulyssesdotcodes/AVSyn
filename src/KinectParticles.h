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

class KinectParticles : public Visualization {
public:
	virtual void setup(AudioSource* audioSource, BeatDetector* beatDetector, map<string, Visualization*> visualizations, vector<string> visualizationOptions);
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

	Visualization* mVisualization;
	map<string, Visualization*> mVisualizations;
	vector<string> mVisualizationOptions;
	int mCurrentVisOption;

	CameraPersp* mCam;
	params::InterfaceGlRef mParams;

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