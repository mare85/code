#include "StaticMesh.h"
#include <Game/Transform.h>
#include <Util/DebugDraw.h>

void Game::StaticMesh::loadData(Graphics::GdiContext * gdiContext)
{
	if( !data_ || !shaderFilename_[0] )
	{
		return;
	}
	if( shaderCachename_[ 0 ] )
	{
		sh_ = gdiContext->createShader(shaderFilename_, &vdesc_, shaderCachename_);
	}
	else
	{
		sh_ = gdiContext->createShader(shaderFilename_, &vdesc_ );
	}
	vBuff_ = gdiContext->createBuffer(data_, vdesc_.strides_[0] * vertCount_, Graphics::BufferType::Vertex);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);
}

void Game::StaticMesh::unloadData(Graphics::GdiContext * gdiContext)
{
	if (sh_ )
	{
		gdiContext->releaseShader(sh_);

	}
	if ( vBuff_)
	{
		gdiContext->releaseBuffer(vBuff_);
		
	}
	if( cBuff_ )
	{
		gdiContext->releaseBuffer(cBuff_);
	}

}

void Game::StaticMesh::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	if( !sh_ || !vBuff_ )
		return;
	Graphics::ConstantBufferData cbData;
	cbData.world_ = getTransform()->getWorldPose();
	renderContext->applyWorldXWrap(cbData.world_);
	float transformX = cbData.world_.getTranslation().getX();
	float centerX = -transformX -renderContext->view_.getTranslation().getX();
	float extendX = 1.0f / renderContext->projection_.getCol0().getX();
	
	if( centerX + extendX> minx_ && centerX - extendX < maxx_ )
	{
		gdiContext->bindShader(sh_, Graphics::eTriangleStrip);
		cbData.view_ = renderContext->view_;
		cbData.projection_ = renderContext->projection_;
		cbData.userData0_ = renderContext->user0_;
		gdiContext->updateBuffer(cBuff_, &cbData);
		gdiContext->setConstantBuffer(cBuff_);
		gdiContext->drawTriangles(vBuff_, vertCount_);
	}
}

void* Game::StaticMesh::allocateData(unsigned int vertCount, Graphics::VertexDesc* vdesc)
{
	vdesc_ = *vdesc;
	vertCount_ = vertCount;
	if (data_)
	{
		delete[] data_;
	}
	data_ = new unsigned char[ vertCount * vdesc_.strides_[0] ];
	return data_;
}

void Game::StaticMesh::setShader(const char* shaderName, const char* shaderCachename /*= nullptr*/)
{
	if( shaderCachename )
	{
		strcpy_s( shaderCachename_, shaderCachename );
	}
	strcpy_s( shaderFilename_, shaderName );
}
