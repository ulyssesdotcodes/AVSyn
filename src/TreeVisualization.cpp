#include "TreeVisualization.h"

#include "cinder\Rand.h"

void TreeVisualization::setup(AudioSource* audioSource, BeatDetector* beatDetector)
{
	mAudioSource = audioSource;
	mBeatDetector = beatDetector;

	// Graphics 

	mPositions = {};
	mColors = {};
	mIndex = 0;
	
	gl::VboRef positionVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mPositions.size() * sizeof(vec3), mPositions.data(), GL_DYNAMIC_DRAW);
	geom::BufferLayout positionBufferLayout;
	positionBufferLayout.append(geom::Attrib::POSITION, 3, sizeof(vec3), 0);

	gl::VboRef colorVbo = gl::Vbo::create(GL_ARRAY_BUFFER, mColors.size() * sizeof(vec4), mColors.data(), GL_DYNAMIC_DRAW);
	geom::BufferLayout colorBufferLayout;
	colorBufferLayout.append(geom::Attrib::COLOR, 4, sizeof(vec4), 0);

	mMesh = gl::VboMesh::create(MAX_LINE_VERTICES, GL_LINES, 
		{ {positionBufferLayout, positionVbo}, {colorBufferLayout, colorVbo} });

	mBatch = gl::Batch::create(mMesh, gl::getStockShader(gl::ShaderDef().color()));

	// LSystem
	mRu[0] = glm::angleAxis(DELTA_ANGLE, vec3(0, 1, 0));
	mRu[1] = glm::angleAxis(-DELTA_ANGLE, vec3(0, 1, 0));
	mRl[0] = glm::angleAxis(DELTA_ANGLE, vec3(1, 0, 0));
	mRl[1] = glm::angleAxis(-DELTA_ANGLE, vec3(1, 0, 0));
	mRh[0] = glm::angleAxis(DELTA_ANGLE, vec3(0, 0, 1));
	mRh[1] = glm::angleAxis(-DELTA_ANGLE, vec3(0, 0, 1));

	resetGen();
}

void TreeVisualization::switchParams(params::InterfaceGlRef params) {}

void TreeVisualization::switchCamera(CameraPersp mCam) {}

bool TreeVisualization::perspective() 
{
	return true;
}

void TreeVisualization::draw()
{
	gl::lineWidth(4.0f);
	mBatch->draw();
}

void TreeVisualization::runCommand(char rule, Gen* gen) 
{
	if (rule == 'F') {
		mPositions[mIndex++] = gen->currentVertex;
		gen->currentVertex =
			vec3(gen->currentVertex[0] + gen->heading[0] * LENGTH,
				gen->currentVertex[1] + gen->heading[1] * LENGTH,
				gen->currentVertex[2] + gen->heading[2] * LENGTH);
		mPositions[mIndex++] = gen->currentVertex;

		for (int i = 0; i < 2; ++i) {
			int j = mIndex - 2 + i;
			mColors[j] = vec4(Rand::randVec3(), 1.0);
		}
		return;
	}

	vec3 z = vec3(0, 0, 1);
	quat q = quat(gen->heading, z);
	switch (rule)
	{
	case '+':
		gen->heading = glm::rotate(glm::inverse(q), glm::rotate(mRu[0], z));
		break;
	case '-':
		gen->heading = glm::rotate(glm::inverse(q), glm::rotate(mRu[1], z));
		break;
	case '&':
		gen->heading = glm::rotate(glm::inverse(q), glm::rotate(mRl[0], z));
		break;
	case '^':
		gen->heading = glm::rotate(glm::inverse(q), glm::rotate(mRl[1], z));
		break;
	}
}

void TreeVisualization::resetGen()
{
	mPositions = {};
	mColors = {};

	mGenStack.clear();
	Gen* gen = new Gen();
	*gen = { "F", 0, vec3(8, 0, 0), vec3(1, 0, 0), -1 };
	mGenStack.push_back(gen);
	gen = new Gen();
	*gen = {"F", 0, vec3(-8, 0, 0), vec3(-1, 0, 0), -1};
	mGenStack.push_back(gen);
	gen = new Gen();
	*gen = {"F", 0, vec3(0, 8, 0), vec3(0, 1, 0), -1};
	mGenStack.push_back(gen);
	gen = new Gen();
	*gen = {"F", 0, vec3(0, -8, 0), vec3(0, -1, 0), -1};
	mGenStack.push_back(gen);
	gen = new Gen();
	*gen = {"F", 0, vec3(0, 0, 8), vec3(0, 0, 1), -1};
	mGenStack.push_back(gen);
	gen = new Gen();
	*gen = {"F", 0, vec3(0, 0, -8), vec3(0, 0, -1), -1};
	mGenStack.push_back(gen);

	mIndex = 0;
	int stepCount = 0;
	while (stepCount < MAX_LINE_VERTICES / 2) {
		lstep();
		stepCount = 0;
		for (int i = 0; i < mGenStack.size(); ++i) {
			stepCount += mGenStack[i]->str.size();
		}
	}
}

void TreeVisualization::lstep()
{
	for (int i = 0; i < mGenStack.size(); ++i) {
		string newGen = "";
		string gen = mGenStack[i]->str;
		for (int j = 0; j < gen.length(); ++j) {
			if (RULES.find(gen[j]) == RULES.end()) {
				newGen.append(1, gen[j]);
			}
			else {
				vector<string> choices = RULES.at(gen[j]);
				string choice = choices[Rand::randInt(choices.size())];
				newGen.append(choice);
			}
		}

		mGenStack[i]->str = newGen;
	}
}

void TreeVisualization::update()
{
	int j = 0;
	while (mGenStack.size() > j && j < 4) {
		Gen* gen = mGenStack[j];
		if (gen->index >= gen->str.length()) {
			mGenStack.erase(mGenStack.begin() + j);
			continue;
		}

		int i;
		int max = math<int>::min(gen->str.length(), gen->index + 3);
		for (i = gen->index; i < max; ++i) {
			char command = gen->str[i];
			if (command == '[') {
				int len = 0;
				int bracketCount = 0;
				while (!(bracketCount == 0 && gen->str[i + 1 + len] == ']')) {
					bracketCount += gen->str[i + 1 + len] == '[' ? 1 : 0;
					bracketCount -= gen->str[i + 1 + len] == ']' ? 1 : 0;
					len++;
				}

				Gen* newGen = new Gen();
				*newGen = { gen->str.substr(i + 1, len), 0, gen->currentVertex, gen->heading, j };
				mGenStack.push_back(newGen);

				max = math<int>::min(gen->str.length(), max + len);
				i += len + 1;
			}
			else {
				runCommand(command, gen);
			}
		}

		mGenStack[j]->index = i;
		j++;
	}

	if (mIndex > 5000 || mGenStack.size() == 0) {
		resetGen();
	}

	mMesh->bufferAttrib(geom::POSITION, sizeof(vec3) * MAX_LINE_VERTICES, &mPositions[0]);
	mMesh->bufferAttrib(geom::COLOR, sizeof(vec4) * MAX_LINE_VERTICES, &mColors[0]);
}
