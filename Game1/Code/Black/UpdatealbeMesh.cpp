#include "UpdatealbeMesh.h"

#include <Graphics/Shader.h>
#include <Graphics/Shader.h>
#include <Game/Transform.h>


void Black::UpdatealbeMesh::loadData(Graphics::GdiContext * gdiContext)
{
	Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS4);
	sh_ = gdiContext->createShader("assets/Shaders/Black.fx", &vDescPos, "Black");
	vBuff_[0]= gdiContext->createBuffer(nullptr, sizeof( vmath::Vector4 ) * vertCount_, Graphics::BufferType::DynamicVertex);
	vBuff_[1] = gdiContext->createBuffer(nullptr, sizeof(vmath::Vector4) * vertCount_, Graphics::BufferType::DynamicVertex);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);
}

void Black::UpdatealbeMesh::unloadData(Graphics::GdiContext * gdiContext)
{
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_[0]);
	gdiContext->releaseBuffer(vBuff_[1]);
	gdiContext->releaseBuffer(cBuff_);
}

void Black::UpdatealbeMesh::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	gdiContext->bindShader(sh_, Graphics::eTriangleStrip );
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();

	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(vBuff_[currRender_], vertCount_);
}

void Black::UpdatealbeMesh::updateGfx(Graphics::GdiContext * gdiContext)
{
	if( vertsGpu_ )
		gdiContext->unmap(vBuff_[currUpdate_]);
	currUpdate_ = currUpdate_ ^ 1;
	currRender_ = currRender_ ^ 1;
	vertsGpu_ = reinterpret_cast< vmath::Vector4* > ( gdiContext->mapWrite(vBuff_[currUpdate_]) );

	needUpdate = max(needUpdate - 1, 0);
}
