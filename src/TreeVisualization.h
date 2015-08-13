#pragma once

#include "Visualization.h"
#include "AudioSource.h"
#include "BeatDetector.h"
#include "cinder\gl\gl.h"

using namespace ci;

const int MAX_LINE_VERTICES = 5000;
const float DELTA_ANGLE = M_PI * 22.5 / 180.0;
const float LENGTH = 4.0;
const float ROTATION_DAMP = 0.01;

const map<char, vector<string>> RULES = 
{
	{'F', {
		"F[+F]F[-F]F",
		"[+F][-F]",
		"F[-F]F",
		"F[+F]F",
		"F[+F]F[-F]F",
		"[+F][-F]",
		"F[-F]F",
		"F[+F]F",
		"F[&F[+F]F]F",
		"F[&F[-F]F]F",
		"F[^F[+F]F]F",
		"F[^F[-F]F]F"
	}}
};

struct Gen {
	string str;
	int index;
	vec3 currentVertex;
	vec3 heading;
	int parent;
};

class TreeVisualization: public Visualization {
public:
	void setup(AudioSource* audioSource, BeatDetector* beatDetector);
	void update() override;
	void draw() override;
	void switchCamera(CameraPersp* cam) override;
	void switchParams(params::InterfaceGlRef params) override;
	bool perspective() override;

private:
	void resetGen();
	void lstep();
	void runCommand(char rule, Gen* gen);

	AudioSource* mAudioSource;
	BeatDetector* mBeatDetector;

	array<vec3, MAX_LINE_VERTICES> mPositions;
	array<vec4, MAX_LINE_VERTICES> mColors;
	int mIndex;

	gl::GlslProgRef mShader;
	gl::BatchRef mBatch;
	gl::VboMeshRef mMesh;

	array<quat, 2> mRu, mRl, mRh;

	vector<Gen*> mGenStack;

	int mGrowth;
	float mRotationSpeed;
	float mHue;
	float mBeatConstant;

	ColorA mColor;
	quat mRotation;
};