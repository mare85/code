#include "DebugDraw.h"
#include <Graphics/VertexTypes.h>
#include <Graphics/Shader.h>
#include <Graphics/GdiContext.h>
#include <Graphics/RenderContext.h>

Util::DebugDraw* __inst = nullptr;

void Util::DebugDraw::init(Graphics::GdiContext * gdiContext)
{
	if (__inst)
		return;
	__inst = new DebugDraw();
	Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS4COL4);
	__inst->sh_ = gdiContext->createShader("assets/Shaders/DebugDraw.fx", &vDescPos, "DebugDraw");
	__inst->vBuff_ = gdiContext->createBuffer(nullptr, sizeof(vmath::Vector4) * eMaxVerts * 2, Graphics::eDynamicWriteVertexBuffer);
	__inst->cBuff_ = gdiContext->createBuffer(nullptr, sizeof(vmath::Matrix4) * 2, Graphics::eDefaultConstantBuffer);
}

void Util::DebugDraw::release(Graphics::GdiContext * gdiContext)
{
	if (__inst->mappedData_)
	{
		gdiContext->unmap(__inst->vBuff_);
	}
	gdiContext->releaseShader(__inst->sh_);
	gdiContext->releaseBuffer(__inst->vBuff_);
	gdiContext->releaseBuffer(__inst->cBuff_);
	delete __inst;
	__inst = nullptr;
}

void Util::DebugDraw::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	if (__inst->mappedData_ )
	{
		if (__inst->nVertPoints_)
		{
			gdiContext->bindShader(__inst->sh_, Graphics::eLines);
			vmath::Matrix4 cbData[2];
			cbData[0] = renderContext->view_;
			cbData[1] = renderContext->projection_;

			gdiContext->updateBuffer(__inst->cBuff_, cbData);
			gdiContext->setConstantBuffer(__inst->cBuff_);
			gdiContext->drawTriangles(__inst->vBuff_, __inst->nVertPoints_);
		}
		gdiContext->unmap(__inst->vBuff_);
	}
	__inst->nVertPoints_ = 0;
	__inst->mappedData_ = reinterpret_cast< vmath::Vector4* >( gdiContext->mapWrite(__inst->vBuff_) );
	__inst->addPtr_ = __inst->mappedData_;
}

void Util::DebugDraw::addPoint(vmath::Vector3 pos, float size, vmath::Vector4 color)
{
	if (!__inst->mappedData_ || __inst->mappedData_ + eMaxVerts <= __inst->addPtr_ + 12)
		return;
	__inst->addPtr_[0] = vmath::Vector4(pos - vmath::Vector3( size, .0f, .0f), 1.0f);
	__inst->addPtr_[1] = color;
	__inst->addPtr_[2] = vmath::Vector4(pos + vmath::Vector3( size, .0f, .0f), 1.0f);
	__inst->addPtr_[3] = color;

	__inst->addPtr_[4] = vmath::Vector4(pos - vmath::Vector3(.0f, size, .0f), 1.0f);
	__inst->addPtr_[5] = color;
	__inst->addPtr_[6] = vmath::Vector4(pos + vmath::Vector3(.0f, size, .0f), 1.0f);
	__inst->addPtr_[7] = color;

	__inst->addPtr_[8]  = vmath::Vector4(pos - vmath::Vector3(.0f, .0f, size), 1.0f);
	__inst->addPtr_[9]  = color;
	__inst->addPtr_[10] = vmath::Vector4(pos + vmath::Vector3(.0f, .0f, size), 1.0f);
	__inst->addPtr_[11] = color;
	__inst->addPtr_ += 12;
	__inst->nVertPoints_ += 12;
}

void Util::DebugDraw::addLine(vmath::Vector3 pos0, vmath::Vector3 pos1, vmath::Vector4 color)
{
	if (!__inst->mappedData_ || __inst->mappedData_ + eMaxVerts <= __inst->addPtr_ + 4)
		return;
	__inst->addPtr_[0] = vmath::Vector4(pos0, 1.0f);
	__inst->addPtr_[1] = color;
	__inst->addPtr_[2] = vmath::Vector4(pos1, 1.0f);
	__inst->addPtr_[3] = color;
	__inst->addPtr_ += 4;
	__inst->nVertPoints_ += 4;
}
