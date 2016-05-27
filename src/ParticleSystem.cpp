//----------------------------------------------------------------------------------
// File:        ComputeParticles/ParticleSystem.cpp
// Url:         https://github.com/NVIDIAGameWorks/OpenGLSamples/tree/master/samples/es3aep-kepler/ComputeParticles
// SDK Version: v1.2 
// Email:       gameworks@nvidia.com
// Site:        http://developer.nvidia.com/
//
// Copyright (c) 2014, NVIDIA CORPORATION. All rights reserved.
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions
// are met:
//  * Redistributions of source code must retain the above copyright
//    notice, this list of conditions and the following disclaimer.
//  * Redistributions in binary form must reproduce the above copyright
//    notice, this list of conditions and the following disclaimer in the
//    documentation and/or other materials provided with the distribution.
//  * Neither the name of NVIDIA CORPORATION nor the names of its
//    contributors may be used to endorse or promote products derived
//    from this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS ``AS IS'' AND ANY
// EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
// PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR
// CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
// PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
// PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
// OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
//----------------------------------------------------------------------------------

#include "ParticleSystem.h"

#define LIGHT_RADIUS	1.5f // Must be at least 1

using namespace ci;

float sfrand()
{
	return ci::randPosNegFloat( -1.0f, 1.0f );
}

ParticleSystem::ParticleSystem()
	: mCam( app::getWindowWidth(), app::getWindowHeight(), 45.0f, 0.1f, 10.0f ),
	  mNoiseSize( 16 ),
	  mParticleParams( mNoiseSize ),
	  mSpriteSize( 0.01f ),
	  mReset( false ),
	  mStartTime( 0.0f )
{
	mReset = false;
	mStartTime = app::getElapsedSeconds();
	mNoiseSize = 16;

	setupNoiseTexture3D();
	setupShaders();
	setupBuffers();
	resetParticleSystem();

	CI_CHECK_GL();

	mCam.lookAt( vec3( 0.0f, 0.0f, -3.0f ), vec3( 0 ) );
}

void ParticleSystem::setupShaders()
{
	try {
		mRenderProg = gl::GlslProg::create( gl::GlslProg::Format().vertex( app::loadAsset( "render_particles.vert" ) )
			.fragment( app::loadAsset( "render_particles.frag" ) ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
	}

	try {
		mUpdateProg = gl::GlslProg::create( gl::GlslProg::Format().compute( app::loadAsset( "particles.comp" ) ) );
	}
	catch( gl::GlslProgCompileExc e ) {
		ci::app::console() << e.what() << std::endl;
	}

	// Particle update ubo.
	mParticleUpdateUbo = gl::Ubo::create( sizeof( mParticleParams ), &mParticleParams, GL_DYNAMIC_DRAW );
	mParticleUpdateUbo->bindBufferBase( 0 );
	mUpdateProg->uniformBlock( "ParticleParams", 0 );

	mUpdateProg->uniform( "noiseTex3D", 0 );
}

void ParticleSystem::setupBuffers()
{
	mPos = gl::Ssbo::create( sizeof(vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW );
	mVel = gl::Ssbo::create( sizeof(vec4) * NUM_PARTICLES, nullptr, GL_STATIC_DRAW );

	std::vector<uint32_t> indices( NUM_PARTICLES * 6 );
	// the index buffer is a classic "two-tri quad" array.
	// This may seem odd, given that the compute buffer contains a single
	// vector for each particle.  However, the shader takes care of this
	// by indexing the compute shader buffer with a /4.  The value mod 4
	// is used to compute the offset from the vertex site, and each of the
	// four indices in a given quad references the same center point
	for( size_t i = 0, j = 0; i < NUM_PARTICLES; ++i ) {
		size_t index = i << 2;
		indices[j++] = index;
		indices[j++] = index + 1;
		indices[j++] = index + 2;
		indices[j++] = index;
		indices[j++] = index + 2;
		indices[j++] = index + 3;
	}

	mIndicesVbo = gl::Vbo::create<uint32_t>( GL_ELEMENT_ARRAY_BUFFER, indices, GL_STATIC_DRAW );
}

void ParticleSystem::update()
{
	updateParticleSystem();
}

void ParticleSystem::draw()
{
	// CI_CHECK_GL();
	gl::clear( Color( 0.0f, 0.0f, 0.0f) );

	if( mReset ) {
		mReset = false;
		resetParticleSystem();
	}

	gl::setMatrices( mCam );

	// draw particles
	gl::ScopedGlslProg scopedRenderProg( mRenderProg );
	mRenderProg->uniform( "spriteSize", mSpriteSize );

	gl::context()->setDefaultShaderVars();

	gl::enableAdditiveBlending();

	gl::disable( GL_DEPTH_TEST );
	gl::disable( GL_CULL_FACE );

	{
		//ScopedBufferBase scopedPosBuffer( mPos, 1 );
		gl::bindBufferBase( mPos->getTarget(), 1, mPos );
		gl::ScopedBuffer scopedIndicex( mIndicesVbo );
		gl::drawElements( GL_TRIANGLES, NUM_PARTICLES * 6, GL_UNSIGNED_INT, 0 );
	}
	
	gl::disableAlphaBlending();
}

void ParticleSystem::resetParticleSystem()
{
	mParticleParams.end = false;
	float size = 0.5f;
	vec4 *pos = reinterpret_cast<vec4*>( mPos->map( GL_WRITE_ONLY ) );
	for( size_t i = 0; i < NUM_PARTICLES; ++i ) {
		pos[i] = vec4(0.0f, 0.0f, 0.0f, 0.0f);
	}
	mPos->unmap();

	vec4 *vel = reinterpret_cast<vec4*>( mVel->map( GL_WRITE_ONLY ) );
	for( size_t i = 0; i < NUM_PARTICLES; ++i ) {
		vel[i] = vec4( 0.0f, 0.0f, 0.0f, 1.0f );
	}
	mVel->unmap();

	mStartTime = app::getElapsedSeconds();
}

void ParticleSystem::updateParticleSystem()
{
	mParticleParams.numParticles = NUM_PARTICLES;

	// Invoke the compute shader to integrate the particles
	gl::ScopedGlslProg prog( mUpdateProg );

	mParticleUpdateUbo->bufferSubData( 0, sizeof( mParticleParams ), &mParticleParams );
	gl::ScopedTextureBind scoped3dTex( mNoiseTex );

	//ScopedBufferBase scopedPosBuffer( mPos, 1 );
	//ScopedBufferBase scopedVelBuffer( mVel, 2 );
	gl::bindBufferBase( mPos->getTarget(), 1, mPos );
	gl::bindBufferBase( mPos->getTarget(), 2, mVel );

	gl::dispatchCompute( NUM_PARTICLES / WORK_GROUP_SIZE, 1, 1 );
	// We need to block here on compute completion to ensure that the
	// computation is done before we render
	gl::memoryBarrier( GL_SHADER_STORAGE_BARRIER_BIT );
}

void ParticleSystem::setupNoiseTexture3D()
{
	gl::Texture3d::Format tex3dFmt;
	tex3dFmt.setWrapR( GL_REPEAT );
	tex3dFmt.setWrapS( GL_REPEAT );
	tex3dFmt.setWrapT( GL_REPEAT );
	tex3dFmt.setMagFilter( GL_LINEAR );
	tex3dFmt.setMinFilter( GL_LINEAR );
	tex3dFmt.setDataType( GL_FLOAT );
	tex3dFmt.setInternalFormat( GL_RGBA8_SNORM );

	const int width = mNoiseSize;
	const int height = mNoiseSize;
	const int depth = mNoiseSize;

	std::vector<float> data( width * height * depth * 4 );
	int i = 0;
	for( int z = 0; z < depth; ++z ) {
		for( int y = 0; y < height; ++y ) {
			for( int x = 0; x < width; ++x ) {
				data[i++] = sfrand();
				data[i++] = sfrand();
				data[i++] = sfrand();
				data[i++] = sfrand();
			}
		}
	}

	mNoiseTex = gl::Texture3d::create( mNoiseSize, mNoiseSize, mNoiseSize, tex3dFmt );
	mNoiseTex->update( data.data(), GL_RGBA, tex3dFmt.getDataType(), 0, mNoiseTex->getWidth(),
		mNoiseTex->getHeight(), mNoiseTex->getDepth() );
}

void ParticleSystem::setStartPositionX(float x)
{
	mParticleParams.start.x = x;
}

void ParticleSystem::setStartPositionY(float y)
{
	mParticleParams.start.y = y;
}

void ParticleSystem::setStartPosition(ci::vec3 pos)
{
	mParticleParams.start = pos;
}

void ParticleSystem::setStartVelocity(ci::vec3 vel)
{
	mParticleParams.startVel = vel;
}

void ParticleSystem::setLifetime(float lifetime)
{
	mParticleParams.lifetime = lifetime;
}

void ParticleSystem::setTime(float time)
{
	mParticleParams.time = time;
}

void ParticleSystem::end()
{
	mParticleParams.end = true;
}
