#include "GroundMesh.h"
#include <Black/Ground.h>
#include <Graphics/Shader.h>
#include <Game/Transform.h>

void Black::GroundMesh::loadData(Graphics::GdiContext * gdiContext)
{

	typedef Graphics::Semantic S;
	Graphics::VertexDesc vDescPos = {{S::Pos4}};
	sh_ = gdiContext->createShader("assets/Shaders/Black.fx", &vDescPos, "Black");
	const int minScope = -Black::Ground::eNColliders / 4;
	const int maxScope = Black::Ground::eNColliders * 5 / 4;
	const int nSteps = maxScope - minScope;
	vmath::Vector4 positions[nSteps * eNSteps * 2];
	_FillBuffer(positions);

	vBuff_ = gdiContext->createBuffer(positions, sizeof(vmath::Vector4) * vertCount_, Graphics::BufferType::CpuToVertex);
	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);
}

void Black::GroundMesh::unloadData(Graphics::GdiContext * gdiContext)
{
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_);
	gdiContext->releaseBuffer(cBuff_);
}

void Black::GroundMesh::render(Graphics::GdiContext * gdiContext, Graphics::RenderContext * renderContext)
{
	gdiContext->bindShader(sh_, Graphics::eTriangleStrip);
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;



	cbData.world_ = getTransform()->getWorldPose();
	renderContext->applyWorldXWrap(cbData.world_);

	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(vBuff_,vertCount_);
}

void Black::GroundMesh::updateGfx(Graphics::GdiContext * gdiContext)
{
	if (forceUpdate_)
	{	
		_FillBuffer(reinterpret_cast< vmath::Vector4 * >(gdiContext->mapWrite(vBuff_)));
		gdiContext->unmap(vBuff_);
		forceUpdate_ = false;
	}
}

void Black::GroundMesh::_FillBuffer(vmath::Vector4 * buff)
{

	const int minScope = -Black::Ground::eNColliders / 4;
	const int maxScope = Black::Ground::eNColliders * 5 / 4;
	const int nSteps = maxScope - minScope;
	vertCount_ = nSteps * eNSteps * 2;

	
	vmath::Vector4* posPtr = buff;
	float floorY = -100.0f;
	float prevX = ground_->colliders_[Black::Ground::eNColliders + minScope].getXMin();
	float prevX2 = prevX;

	for (int collI = minScope; collI < maxScope; ++collI)
	{
		int collIndex = (collI + Black::Ground::eNColliders) % Black::Ground::eNColliders;
		Black::CubicFunctionColider &collider = ground_->colliders_[collIndex];
		float stepX = collider.getWidth() / (eNSteps);
		prevX = ground_->colliders_[collIndex].getXMin();
		prevX2 = prevX;
		if (collI < 0)
		{
			prevX -= 800.0f;
		}
		else if (collI >= Black::Ground::eNColliders)
		{
			prevX += 800.0f;
		}
		for (unsigned int i = 0; i < eNSteps; ++i)
		{
			float x = prevX + stepX;
			prevX2 += stepX;
			float y = collider.getTopHeight(prevX2);
			posPtr[0] = vmath::Vector4(x, floorY, .0f, 1.0f);
			posPtr[1] = vmath::Vector4(x, y, .0f, 1.0f);
			posPtr += 2;
			prevX = x;
		}
	}

}

Black::GroundMesh::~GroundMesh()
{
	 delete ground_;
}
