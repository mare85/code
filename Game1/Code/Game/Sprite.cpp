#include "Sprite.h"
#include <Game/Transform.h>

namespace Game
{
	float Sprite::_DotsPerUnit = 360.0f;

	void Sprite::loadData(Graphics::GdiContext * gdiContext)
	{
		Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS4UV4);
		sh_ = gdiContext->createShader("assets/Shaders/Sprite.fx", &vDescPos, filename_);

		Graphics::TextureDesc textureDesc;
		textureDesc.filter_ = Graphics::TextureDesc::ePoint;
		textureDesc.mode_ = Graphics::TextureDesc::eClamp;
		texture_ = gdiContext->createTexture(filename_, &textureDesc, filename_);
		
		sh_->setTexture(0, texture_);

		unsigned int w, h;
		texture_->getSize(&w, &h);
		float extendX = w * .5f / _DotsPerUnit;
		float extendY = h * .5f / _DotsPerUnit;
		
		Graphics::VertexP4UV4 points[] = {
			{ vmath::Vector4(extendX, extendY, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
			{ vmath::Vector4(extendX,-extendY, .0f, 1.0f), vmath::Vector4(1.0f, 1.0f, .0f, .0f) },
			{ vmath::Vector4(-extendX,-extendY, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
			{ vmath::Vector4(extendX, extendY, .0f, 1.0f), vmath::Vector4(1.0f, 0.0f, .0f, .0f) },
			{ vmath::Vector4(-extendX,-extendY, .0f, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
			{ vmath::Vector4(-extendX,extendY, .0f, 1.0f), vmath::Vector4(0.0f, 0.0f, .0f, .0f) }
		};
		unsigned int bufferSize = sizeof(Graphics::VertexP4UV4) * 6;
		vBuff_ = gdiContext->createBuffer(points, bufferSize, Graphics::eDefaultVertexBuffer);
		cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::eDefaultConstantBuffer);

	}

	void Sprite::unloadData(Graphics::GdiContext * gdiContext)
	{
		gdiContext->releaseTexture(texture_);
		gdiContext->releaseShader(sh_);
		gdiContext->releaseBuffer(vBuff_);
		gdiContext->releaseBuffer(cBuff_);
	}

	void Sprite::render(Graphics::GdiContext * gdiContext,Graphics::RenderContext* renderContext)
	{
		gdiContext->bindShader( sh_);
		Graphics::ConstantBufferData cbData;
		cbData.view_ = renderContext->view_;
		cbData.projection_ = renderContext->projection_;
		cbData.world_ = getTransform()->getWorldPose();

		gdiContext->updateBuffer(cBuff_, &cbData);
		gdiContext->setConstantBuffer(cBuff_);
		gdiContext->drawTriangles(vBuff_, 6);
	}

}