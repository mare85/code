#include "Postprocess.h"

void Graphics::Postprocess::draw(Graphics::GdiContext * gdiContext, void* cbData)
{
	RenderTarget::draw(sh_, gdiContext, cbData);
}

void Graphics::Postprocess::draw(Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other)
{
	RenderTarget::draw(sh_, gdiContext, cbData, other);
}

void Graphics::Postprocess::draw(Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other, RenderTarget * other2)
{
	RenderTarget::draw(sh_, gdiContext, cbData, other, other2);
}

void Graphics::RenderTarget::draw(Shader * sh, Graphics::GdiContext * gdiContext, void * cbData)
{
	gdiContext->bindShader(sh);
	gdiContext->bindAsTexture(this, 0);
	if (cbData)
	{
		gdiContext->updateBuffer(cBuff_, cbData);
		gdiContext->setConstantBuffer(cBuff_);
	}
	gdiContext->drawTriangles(vbuff_, 6);
	gdiContext->unbindTextureSlot(0);

}

void Graphics::RenderTarget::draw(Shader * sh, Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other)
{
	gdiContext->bindShader(sh);
	gdiContext->bindAsTexture(this, 0);
	gdiContext->bindAsTexture(other, 1);
	if (cbData)
	{
		gdiContext->updateBuffer(cBuff_, cbData);
		gdiContext->setConstantBuffer(cBuff_);
	}
	gdiContext->drawTriangles(vbuff_, 6);
	gdiContext->unbindTextureSlot(0);
	gdiContext->unbindTextureSlot(1);
}

void Graphics::RenderTarget::draw(Shader * sh, Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other, RenderTarget * other2)
{
	gdiContext->bindShader(sh);
	gdiContext->bindAsTexture(this, 0);
	gdiContext->bindAsTexture(other, 1);
	gdiContext->bindAsTexture(other2, 2);
	if (cbData)
	{
		gdiContext->updateBuffer(cBuff_, cbData);
		gdiContext->setConstantBuffer(cBuff_);
	}
	gdiContext->drawTriangles(vbuff_, 6);
	gdiContext->unbindTextureSlot(0);
	gdiContext->unbindTextureSlot(1);
	gdiContext->unbindTextureSlot(2);
}

void Graphics::RenderTarget::draw(Shader * sh, Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other, RenderTarget * other2, RenderTarget * other3)
{
	gdiContext->bindShader(sh);
	gdiContext->bindAsTexture(this, 0);
	gdiContext->bindAsTexture(other, 1);
	gdiContext->bindAsTexture(other2, 2);
	gdiContext->bindAsTexture(other3, 3);
	if (cbData)
	{
		gdiContext->updateBuffer(cBuff_, cbData);
		gdiContext->setConstantBuffer(cBuff_);
	}
	gdiContext->drawTriangles(vbuff_, 6);
	gdiContext->unbindTextureSlot(0);
	gdiContext->unbindTextureSlot(1);
	gdiContext->unbindTextureSlot(2);
	gdiContext->unbindTextureSlot(3);
}

void Graphics::RenderTarget::draw(Shader * sh, Graphics::GdiContext * gdiContext, void * cbData, RenderTarget * other, RenderTarget * other2, RenderTarget * other3, RenderTarget * other4)
{
	gdiContext->bindShader(sh);
	gdiContext->bindAsTexture(this, 0);
	gdiContext->bindAsTexture(other, 1);
	gdiContext->bindAsTexture(other2, 2);
	gdiContext->bindAsTexture(other3, 3);
	gdiContext->bindAsTexture(other4, 4);
	if (cbData)
	{
		gdiContext->updateBuffer(cBuff_, cbData);
		gdiContext->setConstantBuffer(cBuff_);
	}
	gdiContext->drawTriangles(vbuff_, 6);
	gdiContext->unbindTextureSlot(0);
	gdiContext->unbindTextureSlot(1);
	gdiContext->unbindTextureSlot(2);
	gdiContext->unbindTextureSlot(3);
	gdiContext->unbindTextureSlot(4);
}
