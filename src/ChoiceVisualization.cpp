#include "ChoiceVisualization.h"

#include "cinder/app/App.h"

using namespace ci;

ChoiceVisualization::ChoiceVisualization(const World& world, 
	std::map<std::string, std::shared_ptr<Visualization>> visualizations, OscVisController oscVisController) :
	mOscVisController(oscVisController), mVisualizations(visualizations)
{
	mVisualizationName = "Blank";
	mVisualization = visualizations[mVisualizationName];

	mFadeTransition = nullptr;

	gl::GlslProg::Format shaderFmt;
	shaderFmt.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("feedback.frag"));
	mFeedbackShader = gl::GlslProg::create(shaderFmt);
	mFeedbackShader->uniform("i_resolution", (vec2) world.windowSize);
	mFeedbackShader->uniform("i_mirror", 1.0f);

	gl::Texture2d::Format texFmt;
	texFmt.setInternalFormat(GL_RGBA16F);
	texFmt.setDataType(GL_FLOAT);
	texFmt.setTarget(GL_TEXTURE_2D);
	texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
	texFmt.enableMipmapping(false);
	gl::Fbo::Format fmt;
	fmt.disableDepth()
		.setColorTextureFormat(texFmt);

	mCurrentVis = gl::Fbo::create(world.windowSize.x, world.windowSize.y, fmt);
	mPingPongFBO = PingPongFBO(fmt, ivec2(world.windowSize.x, world.windowSize.y) , 2);

	onConnection();

	//addParamName(group + "/Feedback/Reset");
	//params->addButton(group + "/Feedback/Reset",
	//	[=]() {
	//		mFade = 0.98;
	//		mScale = 1.0;
	//		mManipFade = 0.0;
	//	}, "group=" + group);

	//addParamName(group + "/Feedback/ResetColor");
	//params->addButton(group + "/Feedback/ResetColor",
	//	[=]() {
	//		mHueShift = 0.0;
	//		mHueShiftCycle = 0.0;
	//		mSaturationShift = 0.0;
	//		mLightnessShift = 1.0;
	//	}, "group=" + group);
}

void ChoiceVisualization::update(const World& world)
{
	mHueShift = glm::fract(mHueShift + mHueShiftCycle * world.deltaSource->delta());

	// If there's a transition, defer to it.
	if (mFadeTransition != nullptr) {
		mFadeTransition->update(world);
	}
	else {
		mVisualization->update(world);
	}

	// If there's a transition, defer to it
	// TODO: Use types
	{
		gl::ScopedFramebuffer fb(mCurrentVis);
		if (mFadeTransition != nullptr) {
			mFadeTransition->draw(world);
			if (mFadeTransition->isFinished()) {
				mFadeTransition = nullptr;
			}
		}
		else {
			gl::clear(Color(0, 0, 0));
			mVisualization->draw(world);
		}
	}
}

void ChoiceVisualization::draw(const World& world)
{
	if (!mApplyEffects) {
		gl::clear(Color(0, 0, 0));
		gl::setMatricesWindow(world.windowSize);
		gl::draw(mCurrentVis->getColorTexture());
	}
	else {
		{
			mFeedbackShader->uniform("i_hueShift", mHueShift);
			mFeedbackShader->uniform("i_beat", world.beatDetector->getBeat());

			gl::ScopedTextureBind prev(mCurrentVis->getColorTexture(), 0);
			mFeedbackShader->uniform("tex_current", 0);

			gl::ScopedTextureBind current(mPingPongFBO.getTexture(), 1);
			mFeedbackShader->uniform("tex_prev", 1);

			mPingPongFBO.render(mFeedbackShader);
		}

		{
			gl::clear(Color(0, 0, 0));
			gl::pushMatrices();
			gl::setMatricesWindow(world.windowSize);
			gl::draw(mPingPongFBO.getTexture());
			gl::popMatrices();
		}
	}
}

void ChoiceVisualization::setVisualization(std::string name) 
{
	VisualizationRef oldVisualization = std::shared_ptr<Visualization>(mVisualization);

	mOscVisController.clearSliders();

	mVisualizationName = name;
	mVisualization = mVisualizations[mVisualizationName];
	mVisualization->switchParams(mOscVisController);

	if (mFadeTransitionOn) {
		mFadeTransition = std::make_unique<FadeTransition>(oldVisualization, mVisualization, 5.0);
	}
}

void ChoiceVisualization::onConnection()
{
	//mOscVisController.clear();

	mOscVisController.subscribeVisListener([=](std::string name) {
		app::console() << "Received: " << name << std::endl;
		setVisualization(name);
	});

	mApplyEffects = true;
	mOscVisController.subscribeEffectListener("Apply Effects", false, [&](bool enabled) { mApplyEffects = enabled; });

	mFadeTransitionOn = false;
	mOscVisController.subscribeEffectListener("Fade Transition", false, [&](bool enabled) { mFadeTransitionOn = enabled; });

	mHueShift = 0.0;
	mHueShiftCycle = 0.0;

	mOscVisController.subscribeEffectListener("Fade", 0, 1, 0, mFeedbackShader, "i_fade");
	mOscVisController.subscribeEffectListener("Effect Fade", 0, 1, 0, mFeedbackShader, "i_manipFade");
	mOscVisController.subscribeEffectListener("Scale", 0.85, 1.15, 1, mFeedbackShader, "i_scale");
	mOscVisController.subscribeEffectListener("Offset Y", -0.15, 0.15, 0, mFeedbackShader, "i_offsetY");
	mOscVisController.subscribeEffectListener("Hue Shift", 0, 1, 0, [=](float val) { mHueShift = val; });
	mOscVisController.subscribeEffectListener("Hue Shift Cycle", 0, 1, 0, [=](float val) { mHueShiftCycle = val; });
	mOscVisController.subscribeEffectListener("Saturation Shift", 0, 1, 0, mFeedbackShader, "i_saturationShift");
	mOscVisController.subscribeEffectListener("Lightness Shift", 0, 1, 1, mFeedbackShader, "i_lightnessShift");
	mOscVisController.subscribeEffectListener("Beat Expand", -0.5, 0.5, 0, mFeedbackShader, "i_beatExpand");
	mOscVisController.subscribeEffectListener("Beat Rotate", -0.2, 0.2, 0, mFeedbackShader, "i_beatRotate");
	mOscVisController.subscribeEffectListener("Rotate", -0.3, 0.3, 0, mFeedbackShader, "i_rotate");
	mOscVisController.subscribeEffectListener("Mirror", false, [&](bool enabled) { mFeedbackShader->uniform("i_mirror", enabled ? 1.0f : 0.0f); });
}
