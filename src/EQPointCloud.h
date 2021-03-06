#pragma once

#include "cinder/gl/gl.h"

#include "AudioSource.h"
#include "Visualization.h"

struct EQ {
	ci::vec3 pos;
	ci::vec3 velocity;
};

/*
	A cube of particles that respond to the music. The first three particles are moved
	around the cube, each representing a third of the audio spectrum. They influence the
	color of the particles around them in a radius proportional to the volume of that 
	section of the audio spectrum.
*/
class EQPointCloud : public Visualization {

public:
	//! Create the particle buffer and shader program
	EQPointCloud();
	//! Update the three eq particles and the uniforms associated with eqs.
	virtual void update(const World& world);
	//! Draw the particles
	virtual void draw(const World& world);
	virtual void switchParams(OscVisController &controller) override;

private:
	std::vector<ci::vec3> mParticles;
	ci::gl::GlslProgRef mRenderProg;
	ci::gl::VboRef		mParticleBuffer[1];
	ci::gl::BatchRef mBatch;

	std::vector<EQ> mEqs;
	ci::vec3 mEqVolumes;

	float mLoudness;
	ci::quat mRotation;
	float mRotationSpeed;
	float mHue;
	float mLastVolume;
};
