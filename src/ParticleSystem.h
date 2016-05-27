
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


#pragma once

#include "cinder/app/App.h"
#include "cinder/app/RendererGl.h"
#include "cinder/gl/gl.h"
#include "cinder/Camera.h"
#include "cinder/CameraUi.h"
#include "cinder/Utilities.h"
#include "cinder/Rand.h"
#include "cinder/params/Params.h"
#include "cinder/gl/Ssbo.h"
#include "cinder/Arcball.h"


#define LIGHT_RADIUS	1.5f // Must be at least 1

struct ParticleParams
{
	ParticleParams( float noiseSize )
		:
		start(ci::vec3(0.0f, -1.3f, 0.0f)),
		noiseFreq( 10.0f ),
		noiseStrength( 0.001f ),
		invNoiseSize( 1.0f / noiseSize ),
		time(0.0f),
		lifetime(32.0f),
		end(false),
		startVel(ci::vec3(0.0f, 0.0f, 0.0f))
	{
	}

	ci::vec3 start;
	float numParticles;
	float noiseFreq;
	float noiseStrength;
	float invNoiseSize;
	float time;
	float lifetime;
	bool end;
	ci::vec3 startVel;
};

//! This sample was ported from https://github.com/NVIDIAGameWorks/OpenGLSamples/tree/master/samples/es3aep-kepler/ComputeParticles
class ParticleSystem {
  public:
	ParticleSystem();

	void update();
	void draw();
	
	void setupShaders();
	void setupBuffers();
	void resetParticleSystem();
	void updateParticleSystem();
	void setupNoiseTexture3D();
	void setStartPositionX(float x);
	void setStartPositionY(float y);
	void setStartPosition(ci::vec3 pos);
	void setStartVelocity(ci::vec3 vel);
	void setLifetime(float lifetime);
	void setTime(float time);
	void end();

	enum { WORK_GROUP_SIZE = 128, NUM_PARTICLES = 1 << 10 };
	
	ci::gl::VboRef mVBO;
	ci::gl::VboMeshRef teapot;
	ci::gl::GlslProgRef mRenderProg;
	ci::gl::GlslProgRef mUpdateProg;
	ci::gl::SsboRef mPos;
	ci::gl::SsboRef mVel;
	ci::gl::VboRef mIndicesVbo;
	ci::gl::UboRef mParticleUpdateUbo;

	ci::gl::Texture3dRef mNoiseTex;
	ci::params::InterfaceGlRef mParams;
	ci::CameraPersp	mCam;
	ci::CameraUi mCamUi;
	int mNoiseSize;
	ParticleParams mParticleParams;
	float mSpriteSize;
	bool mReset;
	bool mPause;
	float mStartTime;
};
