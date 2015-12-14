#pragma once

#include "cinder/gl/gl.h"

#include "AudioSource.h"
#include "BeatDetector.h"
#include "Visualization.h"

const int MAX_LINE_VERTICES = 5000;
const float DELTA_ANGLE = M_PI * 22.5 / 180.0;
const float LENGTH = 4.0;
const float ROTATION_DAMP = 0.01;

//! The rules for recursively generating an L-System. Each item for a rule has an equal possibility of being used for replacement.
const std::map<char, std::vector<std::string>> RULES = 
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
	std::string str;
	int index;
	ci::vec3 currentVertex;
	ci::vec3 heading;
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
	TreeVisualization();
	//! Run a number of commands based on growth and beat
	void update(const World& world) override;
	//! Draw mPositions with mColors
	void draw(const World& world) override;
	//! Setup params
	void switchParams(OscVisController &controller) override;

private:
	//! Generate the final L system strings and store them in mGen
	void resetGen();
	//! Perform one L system replacement of strings in mGen. Used in resetGen()
	void lstep();
	//! Perform one command - moving forward or turning. Modifies mGen and mPositions.
	void runCommand(char rule, Gen* gen);

	std::array<ci::vec3, MAX_LINE_VERTICES> mPositions;
	std::array<ci::vec4, MAX_LINE_VERTICES> mColors;
	int mIndex;

	ci::gl::GlslProgRef mShader;
	ci::gl::BatchRef mBatch;
	ci::gl::VboMeshRef mMesh;

	std::array<ci::quat, 2> mRu, mRl, mRh;

	std::vector<Gen*> mGenStack;

	int mGrowth;
	float mRotationSpeed;
	float mHue;
	float mBeatConstant;

	ci::ColorA mColor;
	ci::quat mRotation;
};