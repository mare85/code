#include "Event.h"
#include <Graphics/GdiContext.h>
#include <Graphics/VertexTypes.h>
#include <Game/Transform.h>
#include <Game/ParamSet.h>
Black::Event::Event(const char * name)
	: Game::Object(name) 
{ 
	scriptPath[0] = 0; 
	allocParamSet();
	Game::ParamSet* ps = getPaarmSet();
	ps->clear();
	ps->registerFloat("width", &(width_), nullptr);
	ps->registerString("script_path", &(scriptPath), nullptr);
	ps->registerInt("disable_on_enter", &(disableOnEnter), nullptr);
	ps->registerInt("is_text", &(isText_), nullptr);
}
void Black::Event::loadData(Graphics::GdiContext * gdiContext)
{
	Graphics::VertexDesc vdesc = Graphics::VertexDesc::get(Graphics::POS4);
	sh_ = gdiContext->createShader("assets/Shaders/Black.fx", &vdesc, "Black");
	float hh = 3.0f;
	float hw= width_ * .5f;
	vmath::Vector4 positions [] = {
		vmath::Vector4(-hw, -hh, .0f, 1.0f),
		
		vmath::Vector4(hw,  hh, .0f, 1.0f),
		vmath::Vector4(hw, -hh, .0f, 1.0f),
		vmath::Vector4(-hw, -hh, .0f, 1.0f),
		
		vmath::Vector4(-hw,  hh, .0f, 1.0f),
		vmath::Vector4(hw,  hh, .0f, 1.0f),
	};
	vBuff_ = gdiContext->createBuffer(positions, 6 * sizeof(vmath::Vector4), Graphics::eDefaultVertexBuffer);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::eDefaultConstantBuffer);
}

void Black::Event::unloadData(Graphics::GdiContext * gdiContext)
{
	if (sh_)
		gdiContext->releaseShader(sh_);
	if (vBuff_)
		gdiContext->releaseBuffer(vBuff_);
	if (cBuff_)
		gdiContext->releaseBuffer(cBuff_);
}

void Black::Event::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{

	if (!sh_ || !renderContext->inEditor_)
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

void Black::Event::serialize(unsigned char * buffer, unsigned int * outBytes, unsigned int maxBytes)
{
	char data[620];
	float x = getTransform()->getTranslation().getX();
	unsigned int layerId = getLayer();
	sprintf_s(data, "level.event %s %1.2f %d\n", getName(), x, getLayer() );
	if (hasPaarmSet())
	{
		getPaarmSet()->serialize(data, 620, this);
	}
	*outBytes = min((unsigned int)strlen(data) + 1, maxBytes);
	memcpy(buffer, data, *outBytes);
}

void Black::Event::getBounds(float * minVal, float * maxVal)
{
	float x = getTransform()->getTranslation().getX();
	float hw = .5f * width_;
	*minVal = x - hw;
	*maxVal = x + hw;
}
