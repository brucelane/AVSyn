#include "Fluid.h"
#include "cinder\app\App.h"
#include "cinder\Rand.h"

void Fluid::setup()
{
	mWindowResolution = vec2(app::getWindowIndex(0)->getWidth(), app::getWindowIndex(0)->getHeight());
	mFluidResolution = glm::floor(mWindowResolution * vec2(0.2));
	// Setup the shaders
	//vector<string> targetVaryings({ "tf_target" });
	//vector<string> velocityVaryings({ "tf_velocity" });
	//vector<string> pressureVaryings({ "tf_pressure" });
	gl::GlslProg::Format updateFormat;
	///////////
	// Target Varying
	//updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
	//	.feedbackVaryings(targetVaryings);
	updateFormat.vertex(app::loadAsset("passthru.vert"));

	updateFormat.fragment(app::loadAsset("Fluid/advect.frag"));
	mAdvectShader = gl::GlslProg::create(updateFormat);
	mAdvectShader->uniform("resolution", mFluidResolution);

	//updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
	//	.feedbackVaryings(velocityVaryings);
	updateFormat.fragment(app::loadAsset("Fluid/apply_force.frag"));
	mForcesShader = gl::GlslProg::create(updateFormat);
	mForcesShader->uniform("resolution", mFluidResolution);
	mForcesShader->uniform("windowRes", mWindowResolution);
	updateFormat.fragment(app::loadAsset("Fluid/dye_drop.frag"));
	mDyeDropShader = gl::GlslProg::create(updateFormat);
	mDyeDropShader->uniform("resolution", mWindowResolution);

	updateFormat.fragment(app::loadAsset("Fluid/subtract_pressure.frag"));
	mSubtractPressureShader = gl::GlslProg::create(updateFormat);
	mSubtractPressureShader->uniform("resolution", mFluidResolution);

	updateFormat = gl::GlslProg::Format();
	//updateFormat.feedbackFormat(GL_SEPARATE_ATTRIBS)
	//	.feedbackVaryings(pressureVaryings);
	updateFormat.vertex(app::loadAsset("passthru.vert"));
	updateFormat.fragment(app::loadAsset("Fluid/velocity_divergence.frag"));
	mDivergenceShader = gl::GlslProg::create(updateFormat);
	mDivergenceShader->uniform("resolution", mFluidResolution);
	updateFormat.fragment(app::loadAsset("Fluid/solve_pressure.frag"));
	mPressureSolveShader = gl::GlslProg::create(updateFormat);
	mPressureSolveShader->uniform("resolution", mFluidResolution);

	gl::GlslProg::Format renderFormat;
	renderFormat
		.vertex(app::loadAsset("passthru.vert"))
		.fragment(app::loadAsset("Fluid/render.frag"));
	mRenderShader = gl::GlslProg::create(renderFormat);
	mRenderShader->uniform("resolution", mWindowResolution);

	//Surface32f dye(mWindowResolution.x, mWindowResolution.y, false);

	//int n = 0;
	//for (int j = 0; j < mWindowResolution.y; ++j) {
	//	for (int i = 0; i < mWindowResolution.x; ++i) {
	//		// left, top, right, bottom in that order
	//		//connections[n][0] = i > 0 ? n - 1 : -1;
	//		//connections[n][1] = j > 0 ? n - mFluidResolution.x : -1;
	//		//connections[n][2] = i < mFluidResolution.x - 1 ? n + 1 : -1;
	//		//connections[n][3] = j < mFluidResolution.y - 1 ? n + mFluidResolution.x : -1;

	//		//velocities[n] = glm::vec4(Rand::randVec3(), 1.0);
	//		dye.setPixel(ivec2(i, j), Color((float)j / (float)mWindowResolution.y, (float)i / (float)mWindowResolution.x, 0));
	//		n++;
	//	}
	//}


	for (int i = 0; i < 2; ++i) {
		//mVaos[i] = gl::Vao::create();
		//{
		//	gl::ScopedVao scopeVao(mVaos[i]);
		//	mVelocities[i] = gl::Vbo::create(GL_ARRAY_BUFFER, velocities.size() * sizeof(vec4), velocities.data(), GL_STATIC_DRAW);
		//	{
		//		gl::ScopedBuffer scopeBuffer(mVelocities[i]);
		//		gl::vertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		//		gl::enableVertexAttribArray(0);
		//	}

		//	mPressures[i] = gl::Vbo::create(GL_ARRAY_BUFFER, pressures.size() * sizeof(vec2), pressures.data(), GL_STATIC_DRAW);
		//	{
		//		gl::ScopedBuffer scopeBuffer(mPressures[i]);
		//		gl::vertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		//		gl::enableVertexAttribArray(1);
		//	}

		//	mConnections[i] = gl::Vbo::create(GL_ARRAY_BUFFER, connections.size() * sizeof(ivec4), connections.data(), GL_STATIC_DRAW);
		//	{
		//		gl::ScopedBuffer scopeBuffer(mConnections[i]);
		//		gl::vertexAttribIPointer(2, 4, GL_INT, 0, (const GLvoid*)0);
		//		gl::enableVertexAttribArray(2);
		//	}
		//}

		//mDyes[i] = gl::Vbo::create(GL_ARRAY_BUFFER, dye.size() * sizeof(vec4), dye.data(), GL_STATIC_DRAW);

		//mPressureVaos[i] = gl::Vao::create();
		//{
		//	gl::ScopedVao scopedVao(mPressureVaos[i]);
		//	{
		//		gl::ScopedBuffer scopeBuffer(mPressures[i]);
		//		gl::vertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, (const GLvoid*)0);
		//		gl::enableVertexAttribArray(0);
		//	}
		//	{
		//		gl::ScopedBuffer scopeBuffer(mConnections[i]);
		//		gl::vertexAttribIPointer(1, 4, GL_INT, 0, (const GLvoid*)0);
		//		gl::enableVertexAttribArray(1);
		//	}
		//}

		gl::Texture2d::Format texFmt;
		texFmt.setInternalFormat(GL_RGBA32F);
		texFmt.setDataType(GL_FLOAT);
		texFmt.setTarget(GL_TEXTURE_2D);
		texFmt.setWrap(GL_CLAMP_TO_EDGE, GL_CLAMP_TO_EDGE);
		gl::Fbo::Format fmt;
		fmt.disableDepth()
			.setColorTextureFormat(texFmt);
		mVelocityBufTexs[i] = gl::Fbo::create(mFluidResolution.x, mFluidResolution.y, fmt);
		mPressureBufTexs[i] = gl::Fbo::create(mFluidResolution.x, mFluidResolution.y, fmt);
		mDyeBufTexs[i] = gl::Fbo::create(mWindowResolution.x, mWindowResolution.y, fmt);
		{
			gl::ScopedFramebuffer fbo(mVelocityBufTexs[i]);
			gl::clear(Color(0, 0, 0));
		}
		{
			gl::ScopedFramebuffer fbo(mPressureBufTexs[i]);
			gl::clear(Color(0, 0, 0));
		}
		{
			//auto tex = gl::Texture::create(dye);
			gl::ScopedFramebuffer fbo(mDyeBufTexs[i]);
			gl::clear(Color(1, 1, 1));
			gl::ScopedViewport vp(ivec2(0), mDyeBufTexs[i]->getSize());
			gl::pushMatrices();
			gl::setMatricesWindow(mDyeBufTexs[i]->getSize());
			gl::disableDepthRead();
			gl::disableDepthWrite();
			//gl::draw(tex);
			gl::popMatrices();
		}
	}

}

void Fluid::update()
{
	float time = app::getElapsedSeconds();
	float dt = time - mLastTime;
	mLastTime = time;

	advect(dt);

	applyForce(dt);

	computeDivergence();
	solvePressure();
	subtractPressure();

	advectDye(dt, time);
}

void Fluid::mouseDrag(app::MouseEvent mouseEvent) 
{
	if (mLastMouse.x != 0 && mLastMouse.y != 0) {
		mForcesShader->uniform("isMouseDown", mouseEvent.isLeftDown());
		mForcesShader->uniform("mouse", vec2(mouseEvent.getPos()) / mWindowResolution);
		mForcesShader->uniform("lastMouse", mLastMouse / mWindowResolution);

		mDyeDropShader->uniform("isMouseDown", mouseEvent.isLeftDown());
		mDyeDropShader->uniform("lastMouse", mLastMouse / mWindowResolution);
		mDyeDropShader->uniform("mouse", vec2(mouseEvent.getPos()) / mWindowResolution);
	}
	mLastMouse = mouseEvent.getPos();
}

void Fluid::mouseUp(app::MouseEvent mouseEvent) 
{
	if (!mouseEvent.isLeftDown()) {
		mLastMouse = vec2(0);
	}
}

void Fluid::draw()
{
	gl::ScopedTextureBind scopeVel(mDyeBufTexs[mDyeIteration & 1]->getColorTexture(), 2);
	mRenderShader->uniform("tex_dye", 2);
	//gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
	//mRenderShader->uniform("tex_dye", 0);
	//gl::ScopedTextureBind scopePres(mPressureBufTexs[mPressureIteration & 1]->getColorTexture(), 1);
	//mRenderShader->uniform("tex_dye", 1);
	gl::ScopedGlslProg glsl(mRenderShader);
	//gl::ScopedVao scopedVao(mVaos[mIteration & 1]);
	gl::context()->setDefaultShaderVars();

	gl::drawSolidRect(app::getWindowBounds());
}

bool Fluid::perspective()
{
	return false;
}

void Fluid::switchCamera(CameraPersp * camera)
{
}

void Fluid::switchParams(params::InterfaceGlRef params)
{
}

void Fluid::advect(float dt)
{
	mAdvectShader->uniform("dt", dt);
	//gl::ScopedTextureBind scopeTarget(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 1);
	//mAdvectShader->uniform("tex_target", 1);
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
	mAdvectShader->uniform("tex_velocity", 0);
	mAdvectShader->uniform("tex_target", 0);
	mAdvectShader->uniform("boundaryConditions", true);
	mAdvectShader->uniform("target_resolution", mFluidResolution);

	renderToBuffer(mAdvectShader, mVelocityBufTexs[(mIteration + 1) & 1]);
	++mIteration;
}

void Fluid::advectDye(float dt, float time) 
{
	{
		mAdvectShader->uniform("target_resolution", mWindowResolution);
		mAdvectShader->uniform("dt", dt);
		gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
		mAdvectShader->uniform("tex_velocity", 0);
		gl::ScopedTextureBind scopeDye(mDyeBufTexs[mDyeIteration & 1]->getColorTexture(), 2);
		mAdvectShader->uniform("tex_target", 2);
		mAdvectShader->uniform("boundaryConditions", false);

		renderToBuffer(mAdvectShader, mDyeBufTexs[(mDyeIteration + 1) & 1]);
		++mDyeIteration;
	}


	{
		gl::ScopedTextureBind scopeDyeDrop(mDyeBufTexs[mDyeIteration & 1]->getColorTexture(), 2);
		mDyeDropShader->uniform("time", time);
		mDyeDropShader->uniform("tex_prev", 2);
		renderToBuffer(mDyeDropShader, mDyeBufTexs[(mDyeIteration + 1) & 1]);
		++mDyeIteration;

		mDyeDropShader->uniform("isMouseDown", false);
	}
}

void Fluid::applyForce(float dt)
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
	mForcesShader->uniform("tex_velocity", 0);
	mForcesShader->uniform("dt", dt);
	mForcesShader->uniform("time", mLastTime);
	renderToBuffer(mForcesShader, mVelocityBufTexs[(mIteration + 1) & 1]);
	++mIteration;

	mForcesShader->uniform("isMouseDown", false);
}

void Fluid::computeDivergence()
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
	mDivergenceShader->uniform("tex_velocity", 0);
	gl::ScopedTextureBind scopePressure(mPressureBufTexs[mPressureIteration & 1]->getColorTexture(), 1);
	mDivergenceShader->uniform("tex_pressure", 1);

	renderToBuffer(mDivergenceShader, mPressureBufTexs[(mPressureIteration + 1) & 1]);
	++mPressureIteration;
}

void Fluid::solvePressure()
{
	int i;
	for (i = mPressureIteration; i < mPressureIteration + 60; ++i) {
		gl::ScopedTextureBind scopeVel(mPressureBufTexs[i & 1]->getColorTexture(), 1);
		mPressureSolveShader->uniform("tex_pressure", 1);
		renderToBuffer(mPressureSolveShader, mPressureBufTexs[(i + 1) & 1]);
	}
	mPressureIteration = i;
}

void Fluid::subtractPressure()
{
	gl::ScopedTextureBind scopeVel(mVelocityBufTexs[mIteration & 1]->getColorTexture(), 0);
	mSubtractPressureShader->uniform("tex_velocity", 0);
	gl::ScopedTextureBind scopePressure(mPressureBufTexs[mPressureIteration & 1]->getColorTexture(), 1);
	mSubtractPressureShader->uniform("tex_pressure", 1);

	renderToBuffer(mSubtractPressureShader, mVelocityBufTexs[(mIteration + 1) & 1]);
	++mIteration;
}

void Fluid::renderToBuffer(gl::GlslProgRef shader, gl::FboRef target)
{
	gl::ScopedFramebuffer fbo(target);
	gl::clear(Color(0, 0, 0));
	gl::ScopedViewport vp(ivec2(0), target->getSize());
	gl::pushMatrices();
	gl::setMatricesWindow(target->getSize());
	gl::ScopedGlslProg glsl(shader);

	gl::drawSolidRect(target->getBounds());

	gl::popMatrices();
}
