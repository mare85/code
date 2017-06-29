#pragma once

#include <Util/Utils.h>
#include <Graphics/GdiContext.h>

namespace Graphics
{
	class RenderContext;
	class Shader;
}

namespace Util
{
	class DebugDraw
	{
		enum
		{
			eMaxVerts = 6000
		};
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_;
		Graphics::Buffer* cBuff_;

		vmath::Vector4* mappedData_ = nullptr;
		vmath::Vector4* addPtr_ = nullptr;

		unsigned int nVertPoints_ = 0;
		unsigned char currRender_ = 0;
		unsigned char currUpdate_ = 1;
		char needUpdate = 2;

		DebugDraw() {}
		~DebugDraw() {}

	public:

		static void init(Graphics::GdiContext* gdiContext);
		static void release(Graphics::GdiContext* gdiContext);

		static void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext * renderContext);
		static void addPoint( vmath::Vector3 pos, float size, vmath::Vector4 color);
		static void addLine(vmath::Vector3 pos0, vmath::Vector3 pos1, vmath::Vector4 color);
		
	};

}
