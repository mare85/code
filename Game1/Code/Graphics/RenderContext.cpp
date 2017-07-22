#include "RenderContext.h"

void Graphics::RenderContext::applyWorldXWrap(vmath::Matrix4 & world) const
{
	if (xWrapEnabled_)
	{
		float xObj = world.getCol3().getX();
		float xCam = -view_.getCol3().getX();
		float diff = xObj - xCam;
		float wrapLimit = xWrap_ * 1.3f;
		float wrap2 = xWrap_ * 2.0f;
		if (diff > wrapLimit)
		{
			world.setTranslation(world.getTranslation() + vmath::Vector3(-wrap2, .0f, .0f));
		}
		else if (diff < -wrapLimit)
		{
			world.setTranslation(world.getTranslation() + vmath::Vector3(wrap2, .0f, .0f));
		}
	}
}
