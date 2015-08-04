#include "EQPointCloud.h"

#include "cinder\Color.h"
#include "cinder\Rand.h"
#include "cinder\app\App.h"
#include <vector>

using namespace std;
using namespace ci;

const int NUM_PARTICLES = 3000000;
const int SIZE = 100;

void EQPointCloud::setup(AudioSource audioSource)
{
	mAudioSource = audioSource;

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

void EQPointCloud::switchCamera(CameraPersp cam) {
	cam.lookAt(vec3(0.0, 100.0, 400.0), vec3(0.0));
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

	auto eqVolumes = mAudioSource.getEqs(3);
	mEqVolumes = vec3(eqVolumes.at(0), eqVolumes.at(1), eqVolumes.at(2));
}

void EQPointCloud::draw()
{
	mRenderProg->uniform("eqs", mEqVolumes);
	mRenderProg->uniform("eq0", mEqs.at(0).pos);
	mRenderProg->uniform("eq1", mEqs.at(1).pos);
	mRenderProg->uniform("eq2", mEqs.at(2).pos);

	mBatch->draw();
}

bool EQPointCloud::perspective()
{
	return true;
}
