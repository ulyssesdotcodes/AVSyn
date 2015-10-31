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

//! The rules for recursively generating an L-System. Each item for a rule has an equal possibility of being used for replacement.
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

/*
	A visualization based on L-Systems used to model trees. The basic idea is to generate 6 strings 
	representing completed trees (one for each direction), and then render them branch by branch in
	time to the music. When we run out of vertices, generate a new L-System and start over!
*/
class TreeVisualization: public Visualization {
public:
	//! Setup the sources and the transforms needed for displaying the L systems.
	void setup();
	//! Run a number of commands based on growth and beat
	void update(const World& world) override;
	//! Draw mPositions with mColors
	void draw(const World& world) override;
	//! Setup params
	void switchParams(params::InterfaceGlRef params, const string &group) override;

private:
	//! Generate the final L system strings and store them in mGen
	void resetGen();
	//! Perform one L system replacement of strings in mGen. Used in resetGen()
	void lstep();
	//! Perform one command - moving forward or turning. Modifies mGen and mPositions.
	void runCommand(char rule, Gen* gen);

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