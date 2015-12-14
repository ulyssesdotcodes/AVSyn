#include "EQPointCloud.h"

#include "cinder/app/App.h"
#include "cinder/CinderMath.h"
#include "cinder/Color.h"
#include "cinder/Rand.h"

using namespace ci;

const int NUM_PARTICLES = 300000;
const int SIZE = 64;
const float DAMPING = 0.1;
const float ROTATION_DAMP = 0.1;

EQPointCloud::EQPointCloud()
{
	mLoudness = 0.25;
	mRotationSpeed = 1.0;
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

void EQPointCloud::switchParams(OscVisController &controller) {
	controller.subscribeSliderListener("Loudness", 0, 2, [&](auto val) { mLoudness = val; });
	controller.subscribeSliderListener("Rotation Speed", 0, 2, [&](auto val) { mRotationSpeed = val; });
}

void EQPointCloud::update(const World& world)
{
	for (std::vector<EQ>::iterator it = mEqs.begin(); it != mEqs.end(); ++it) {
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

	world.audioSource->update();
	float volume = world.audioSource->getVolume();
	volume = math<float>::max(volume, mLastVolume * 0.95);
	mLastVolume = volume;
	float rotation = volume * mRotationSpeed * ROTATION_DAMP;
	mRotation = glm::rotate(mRotation, rotation, vec3(1.0, 0.0, 0.0));
	mRotation = glm::rotate(mRotation, rotation * 0.5f, vec3(0.0, 1.0, 0.0));
	auto eqVolumes = world.audioSource->getEqs(3);
	for (int i = 0; i < 3; ++i) {
		mEqVolumes[i] = eqVolumes[i] > mEqVolumes[i] ? eqVolumes[i] * mLoudness : mEqVolumes[i] - (mEqVolumes[i] - eqVolumes[i]) * DAMPING;
	}
}

void EQPointCloud::draw(const World& world)
{
	world.camera->lookAt(vec3(0.0, 0.0, 100.0), vec3(0.0));

	gl::setMatrices(*world.camera);
	gl::enableDepthRead();
	gl::enableDepthWrite();

	mRenderProg->uniform("i_eqs", mEqVolumes);
	mRenderProg->uniform("i_eq0", mEqs.at(0).pos);
	mRenderProg->uniform("i_eq1", mEqs.at(1).pos);
	mRenderProg->uniform("i_eq2", mEqs.at(2).pos);

	gl::rotate(mRotation);
	mBatch->draw();
	gl::rotate(quat());
}