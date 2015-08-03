#include "EQPointCloud.h"

#include "cinder\Color.h"
#include "cinder\Rand.h"
#include <vector>

using namespace std;

const int NUM_PARTICLES = 300000;
const int SIZE = 10000;

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

	auto shaderFormat = gl::GlslProg::Format()
		.vertex(CI_GLSL(150, uniform float	uPointSize;
	uniform mat4	ciModelViewProjection;
	in vec4	ciPosition;

	void main() {
		;
		// modify the point size here with an uniform
		gl_PointSize = clamp(uPointSize * 10.0, 0.001, 10);
		gl_Position = ciModelViewProjection * ciPosition;
	}))
		.fragment(CI_GLSL(150, out vec4 oColor;

	void main() {
		oColor = vec4(smoothstep(0.4f, 0.9f, 1.0f - length(gl_PointCoord.xy - vec2(0.5f))));
	}));

	mRenderProg = gl::GlslProg::create(shaderFormat);

	mAttributes[0] = gl::Vao::create();
	geom::BufferLayout layout;
	layout.append(geom::POSITION, 3, 0, 0);
	auto vbo = gl::Vbo::create(GL_ARRAY_BUFFER, particles.size() * sizeof(vec3), particles.data());
	auto vboMesh = gl::VboMesh::create(particles.size(), GL_POINTS, { {layout, vbo} });
	mBatch = gl::Batch::create(vboMesh, mRenderProg);
	//gl::ScopedVao vao(mAttributes[0]);
	//gl::ScopedBuffer buffer(mParticleBuffer[0]);
	//gl::enableVertexAttribArray(0);
	////gl::enableVertexAttribArray(1);
	//gl::vertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), 0);
	////gl::vertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, sizeof(Particle), (const GLvoid*)offsetof(Particle, color));
}

void EQPointCloud::update()
{

}

void EQPointCloud::draw()
{
	gl::ScopedGlslProg render(mRenderProg);
	mBatch->getGlslProg()->uniform("uPointSize", 10.0f);
	//gl::ScopedVao vao(mAttributes[0]);
	//gl::context()->setDefaultShaderVars();
	//gl::drawArrays(GL_POINTS, 0, NUM_PARTICLES);

	gl::ScopedState programPointSize(GL_PROGRAM_POINT_SIZE, true);
	mBatch->draw();
}
