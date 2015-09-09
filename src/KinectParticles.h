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

/*
	Transforms a different visualization into a visualization that responds to Kinect
	data. It renders the visualization as particles then uses the body image Kinect
	channel to force particles away from where the users are standing. As the users
	move, the particles converge back to their home positions using a velocity function.
*/
class KinectParticles : public Visualization {
public:
	//! Pass in all the sources and also the visualizations that should be turned into a particle field
	virtual void setup(AudioSource* audioSource, BeatDetector* beatDetector, map<string, Visualization*> visualizations, vector<string> visualizationOptions);
	//! Render the secondary visualization to an FBO, transform that FBO into particles, and Kinect it!
	virtual void update();
	//! Draw the particles and the kinect body image
	virtual void draw();
	virtual void switchCamera(CameraPersp* camera);
	virtual void switchParams(params::InterfaceGlRef params, const string &group);
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