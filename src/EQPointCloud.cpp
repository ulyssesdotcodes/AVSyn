#include "EQPointCloud.h"

#include "cinder\Color.h"
#include "cinder\Rand.h"
#include "cinder\app\App.h"
#include "cinder\CinderMath.h"
#include <vector>

using namespace std;
using namespace ci;

const int NUM_PARTICLES = 300000;
const int SIZE = 64;
const float DAMPING = 0.1;
const float ROTATION_DAMP = 0.1;

void EQPointCloud::setup(AudioSource* audioSource)
{
	mLoudness = 0.25;
	mRotationSpeed = 1.0;
	mAudioSource = audioSource;
	mHue = 0.0;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		float x = (Rand::randFloat() - 0.5f) * SIZE;
		float y = (Rand::randFloat() - 0.5f) * SIZE;
		float z = (Rand::randFloat() - 0.5f) * SIZE;

		mParticles.push_back(vec3(x, y, z));
	}

	geom::BufferLayout layout;
	layout.append(geom::POSITION, 3, 0, 0);
	mParticleBuffer[0] = gl::Vbo::create(GL_ARRAY_BUFFER, mParticles.size() * sizeof(vec3), mParticles.data());
	auto vboMesh = gl::VboMesh::create(mParticles.size(), GL_POINTS, { {layout, mParticleBuffer[0]} });
	mRenderProg = gl::GlslProg::create(app::loadAsset("eq_pointcloud.vert"), app::loadAsset("color.frag"));
	mBatch = gl::Batch::create(vboMesh, mRenderProg);
	gl::pointSize(1.0f);
	gl::color(1.0, 1.0, 1.0);

	mEqs.push_back({ mParticles.at(0), Rand::randVec3()});
	mEqs.push_back({ mParticles.at(1), Rand::randVec3()});
	mEqs.push_back({ mParticles.at(2), Rand::randVec3()});
}

void EQPointCloud::switchCamera(CameraPersp* cam) {
	cam->lookAt(vec3(0.0, 0.0, 100.0), vec3(0.0));
}

void EQPointCloud::switchParams(params::InterfaceGlRef params, const string &group) {
	addParamName(group + "/Loudness");
	params->addParam(group + "/Loudness", &mLoudness)
		.min(0.0)
		.max(2.0)
		.step(0.001)
		.group(group);

	addParamName(group + "/Hue");
	params->addParam(group + "/Hue", &mHue)
		.min(0.0)
		.max(1.0)
		.step(0.01)
		.group(group);

	addParamName(group + "/Rotation Speed");
	params->addParam(group + "/Rotation Speed", &mRotationSpeed)
		.min(0.0)
		.max(2.0)
		.step(0.01)
		.group(group);
}

void EQPointCloud::update()
{
	for (vector<EQ>::iterator it = mEqs.begin(); it != mEqs.end(); ++it) {
		it->pos += it->velocity;
		if (math<float>::abs(it->pos.x) > SIZE * 0.5) {
			it->velocity.x = -it->velocity.x;
		}

		if (abs(it->pos.y) > SIZE * 0.5) {
			it->velocity.y = -it->velocity.y;
		}

		if (abs(it->pos.z) > SIZE * 0.5) {
			it->velocity.z = -it->velocity.z;
		}
	}

	mParticles[0] = mEqs[0].pos;
	mParticles[1] = mEqs[1].pos;
	mParticles[2] = mEqs[2].pos;

	mParticleBuffer[0]->copyData(mParticles.size() * sizeof(vec3), mParticles.data());

	mAudioSource->update();
	float volume = mAudioSource->getVolume();
	volume = math<float>::max(volume, mLastVolume * 0.95);
	mLastVolume = volume;
	float rotation = volume * mRotationSpeed * ROTATION_DAMP;
	mRotation = glm::rotate(mRotation, rotation, vec3(1.0, 0.0, 0.0));
	mRotation = glm::rotate(mRotation, rotation * 0.5f, vec3(0.0, 1.0, 0.0));
	auto eqVolumes = mAudioSource->getEqs(3);
	for (int i = 0; i < 3; ++i) {
		mEqVolumes[i] = eqVolumes[i] > mEqVolumes[i] ? eqVolumes[i] * mLoudness : mEqVolumes[i] - (mEqVolumes[i] - eqVolumes[i]) * DAMPING;
	}
}

void EQPointCloud::draw()
{
	mRenderProg->uniform("i_eqs", mEqVolumes);
	mRenderProg->uniform("i_eq0", mEqs.at(0).pos);
	mRenderProg->uniform("i_eq1", mEqs.at(1).pos);
	mRenderProg->uniform("i_eq2", mEqs.at(2).pos);
	mRenderProg->uniform("i_hue", mHue);

	gl::rotate(mRotation);
	mBatch->draw();
}

bool EQPointCloud::perspective()
{
	return true;
}
