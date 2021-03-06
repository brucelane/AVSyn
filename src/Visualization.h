#pragma once

#include "cinder/Camera.h"
#include "cinder/params/Params.h"

#include "World.h"
#include "AudioSource.h"
#include "DeltaSource.h"
#include "OscVisController.h"

typedef std::shared_ptr<class Visualization> VisualizationRef;

/*
  The base Visualiation intended to be extended.
*/
class Visualization 
{
public:
	//! Perform all computations for the visualization.
	virtual void update(const World& world) = 0;
	//! Draw the visualization to the screen. This assumes matrices and viewport have been set up already.
	virtual void draw(const World& world) = 0;
	//! Set up the parameters for this visualization when the user switches to it.
	virtual void switchParams(OscVisController &visController) = 0;
	virtual void hide();
};
