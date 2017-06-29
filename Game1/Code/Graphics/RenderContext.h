#pragma once

#include <Util/Utils.h>
namespace Graphics
{
	class RenderContext
	{
	public:
		vmath::Matrix4 projection_;
		vmath::Matrix4 view_;
		vmath::Vector4 user0_;
		float xWrap_;
		bool xWrapEnabled_;
		bool inEditor_ = false;

		void applyWorldXWrap(vmath::Matrix4& world) const;
	};
}