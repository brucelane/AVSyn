#pragma once

#include "cinder\gl\gl.h"
#include "AudioSource.h"
#include "Visualization.h"

using namespace ci;

class ShaderVisualization : public Visualization {
public:
	ShaderVisualization();
	void setup(const fs::path &fragmentShader);
	virtual void update() override;
	virtual void draw() override;
	virtual void switchCamera(CameraPersp cam) override;
	bool perspective() override;

protected:
	virtual void renderUniforms();
	gl::GlslProgRef mShader;
};
