#include "BlackSprite.h"
#include <Game/Transform.h>
#include <Game/ParamSet.h>
#include <Game/App.h>


void Black::BlackSprite::loadData(Graphics::GdiContext* gdiContext)
{
	Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS4);
	sh_ = gdiContext->createShader("assets/Shaders/BlackSprite.fx", &vDescPos, filename_);

	Graphics::TextureDesc textureDesc;
	textureDesc.filter_ = Graphics::TextureDesc::ePoint;
	textureDesc.mode_ = Graphics::TextureDesc::eClamp;
	texture_ = gdiContext->createTexture(filename_, &textureDesc, filename_);

	
	sh_->setTexture(0, texture_);

	unsigned int w, h;
	texture_->getSize(&w, &h);
	
	if (height_ == .0f)
	{
		height_ = h * 0.003f;
	}

	float extendX = height_ * w * .5f / h ;
	float extendY2 = .5f * height_ + elevation;
	float extendY1 = -.5f * height_ + elevation;
	

	vmath::Vector4 points[] = {
		vmath::Vector4(extendX, extendY2, 1.0f, .0f),
		vmath::Vector4(extendX,extendY1, 1.0f, 1.0f),
		vmath::Vector4(-extendX,extendY1, .0f, 1.0f),
		vmath::Vector4(extendX, extendY2, 1.0f, 0.0f),
		vmath::Vector4(-extendX,extendY1, .0f, 1.0f),
		vmath::Vector4(-extendX,extendY2, .0f, 0.0f)
	};
	unsigned int bufferSize = sizeof(vmath::Vector4) * 6;
	vBuff_ = gdiContext->createBuffer(points, bufferSize, Graphics::eDefaultVertexBuffer);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::eDefaultConstantBuffer);

}
void Black::BlackSprite::unloadData(Graphics::GdiContext* gdiContext)
{
	gdiContext->releaseTexture(texture_);
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_);
	gdiContext->releaseBuffer(cBuff_);
}

void Black::BlackSprite::render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{
	if (!sh_)
		return;
	gdiContext->bindShader(sh_);
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();

	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(vBuff_, 6);
}

void Black::BlackSprite::serialize(unsigned char * buffer, unsigned int * outBytes, unsigned int maxBytes )
{
	char data[300];
	float x = getTransform()->getTranslation().getX();
	sprintf_s(data, "level.sprite %s %d %f %s\n", getName(), layer_, x, filename_);
	if (hasPaarmSet())
	{
		getPaarmSet()->serialize(data, 220, this);
	}
	*outBytes = min((unsigned int)strlen(data) + 1, maxBytes);
	memcpy(buffer, data, *outBytes);

}

Black::BlackSprite::BlackSprite(const char * name, const char * filename, int layer, float height)
	: Object(name)
	, height_(height)
{
	
	strcpy_s(filename_, filename); layer_ = layer;
	allocParamSet();
	Game::ParamSet* ps = getPaarmSet();
	ps->clear();
	ps->registerFloat( "height", &(height_), __SetParamCallback);
	ps->registerFloat( "elevation", &(elevation), __SetParamCallback);
}

void Black::BlackSprite::__SetParamCallback(Game::Object * obj)
{
	Game::App::Instance()->requestReloadLayerGraphics(obj->getLayer());
}
