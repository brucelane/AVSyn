#include "Lights.h"

#include "cinder\app\App.h"
#include "cinder\Rand.h"

using namespace ci;

Lights::Lights() : ShaderVisualization("texture.frag"), mFrequency(1.0)
{
	mResolution = app::getWindowIndex(0)->getSize();

	gl::GlslProgRef newLightShader = gl::getStockShader(gl::ShaderDef().color());
	mNewLight = gl::Batch::create(geom::Circle().radius(100.0), newLightShader);
}

void Lights::update(const World& world)
{
}

void Lights::draw(const World& world)
{
	gl::clear(Color(0, 0, 0));
	gl::ScopedMatrices scp();
	gl::setMatricesWindow(world.windowSize);
	gl::ScopedViewport vp(ivec2(0), world.windowSize);
	float volume = world.audioSource->getVolume();
	float rand = Rand::randFloat();

	// Base the chance for a new light on the volume
	if (rand > volume * 0.5 * mFrequency) {
		return;
	}

	float lightness = volume * 2.0;
	vec2 newLightLocation = vec2(Rand::randFloat() * mResolution.x, Rand::randFloat() * mResolution.y);
	gl::ScopedColor color(Colorf(Rand::randFloat() * lightness, Rand::randFloat() * lightness, Rand::randFloat() * lightness));
	gl::translate(newLightLocation);
	mNewLight->draw();
	gl::translate(vec2(0, 0));
}

void Lights::switchParams(OscVisController &controller)
{
	controller.subscribeSliderListener("Frequency", 0, 4, [&](float val) { mFrequency = val; });
}
