#include "EQPointCloud.h"

#include "cinder\Color.h"
#include "cinder\Rand.h"
#include <vector>

using namespace std;

const int NUM_PARTICLES = 300000;
const int SIZE = 1000;

struct Particle {
	vec3 pos;
	ColorA color;
};

void EQPointCloud::setup(AudioSource audioSource)
{
	vector<vec3> particles;

	for (int i = 0; i < NUM_PARTICLES; i++) {
		float x = (Rand::randFloat() - 0.5f) * SIZE;
		float y = (Rand::randFloat() - 0.5f) * SIZE;
		float z = (Rand::randFloat() - 0.5f) * SIZE;

		particles.push_back(vec3(x, y, z));
	}

	mParticleBuffer[0] = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size() * sizeof(vec3), particles.data(), GL_STATIC_DRAW);

	geom::BufferLayout layout;
	layout.append(geom::POSITION, 3, 0, 0);
	auto vbo = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size() * sizeof(vec3), particles.data());
	auto vboMesh = gl::VboMesh::create(particles.size(), GL_POINTS, { {layout, vbo} });
	mBatch = gl::Batch::create(vboMesh, gl::getStockShader(gl::ShaderDef().color()));
	gl::pointSize(1.0f);
	gl::color(1.0, 1.0, 1.0);
}

void EQPointCloud::update()
{

}

void EQPointCloud::draw()
{
	mBatch->draw();
}

bool EQPointCloud::perspective()
{
	return true;
}
