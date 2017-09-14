#include <GameUtils/SpectrumVisualisation.h>
/*
 * (C) Copytight 2017 Marek Bielawski
 * GameUtils::SpectrumVisualisation
 */
#include <Sound/Spectrum.h>
#include <Sound/Mixer.h>
#include <Game/Transform.h>
namespace GameUtils {

void SpectrumVisualisation::loadData(Graphics::GdiContext* gdiContext) 
{
	Graphics::VertexDesc vDes = {{ Graphics::Semantic::Pos3 }};
	sh_ = gdiContext->createShader("assets/Shaders/WhiteSolid.fx", 
		&vDes, "WhiteSolid");
	const unsigned int nBars = Sound::Spectrum::WindowSize;
	unsigned int nVerts = 4 * nBars;
	unsigned int nIndices = 12 * nBars; // two sided
	unsigned int* indicesData = new unsigned int[ nIndices ];
	unsigned int* indicesPtr = indicesData;
	for( unsigned int i = 0; i < nBars; ++i )
	{
		unsigned int i4 = i<<2;
		indicesPtr[ 0 ] = i4; indicesPtr[ 1 ] = i4 + 1; indicesPtr[ 2 ] = i4 + 2;
		indicesPtr[ 3 ] = i4; indicesPtr[ 4 ] = i4 + 2; indicesPtr[ 5 ] = i4 + 3;
		indicesPtr += 6;
		indicesPtr[ 0 ] = i4; indicesPtr[ 1 ] = i4 + 2; indicesPtr[ 2 ] = i4 + 1;
		indicesPtr[ 3 ] = i4; indicesPtr[ 4 ] = i4 + 3; indicesPtr[ 5 ] = i4 + 2;
		indicesPtr += 6;
	}
	iBuff_ = gdiContext->createBuffer( indicesData, 
		nIndices * sizeof( unsigned int), Graphics::BufferType::Index );
	vPos_ = gdiContext->createBuffer( nullptr, nVerts * 12, Graphics::BufferType::CpuToVertex );
	delete [] indicesData;
	cBuff_ = gdiContext->createBuffer( nullptr, sizeof( Graphics::ConstantBufferData), 
		Graphics::BufferType::Constant );
	Sound::Mixer::Instance()->initSpectrum();
}

void SpectrumVisualisation::unloadData(Graphics::GdiContext* gdiContext) 
{
	Sound::Mixer::Instance()->freeSpectrum();
	gdiContext->releaseBuffer( cBuff_ );
	gdiContext->releaseBuffer( iBuff_ );
	gdiContext->releaseBuffer( vPos_ );
	gdiContext->releaseShader( sh_ );
}

void SpectrumVisualisation::render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) 
{
	const unsigned int nBars = Sound::Spectrum::WindowSize;
	gdiContext->bindShader( sh_ );
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();
	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles(iBuff_, vPos_, nBars * 12 );
}

void SpectrumVisualisation::updateGfx(Graphics::GdiContext* gdiContext) 
{
	const unsigned int nBars = Sound::Spectrum::WindowSize>>6;
	float spectrum[ Sound::Spectrum::WindowSize ];
	Sound::Mixer::Instance()->getSpectrum( reinterpret_cast< float* >( &spectrum ) );
	float* pos = reinterpret_cast< float* >( gdiContext->mapWrite( vPos_ ) );
	struct float6 { float f0,f1,f2,f3,f4,f5; };
	float6* posPtr = reinterpret_cast<float6*>( pos );
	float barStep = width_ / nBars;
	float spreadX = barStep * .45f;
	float barX = width_ * -.5f + barStep * .5f;
	for( unsigned int i = 0; i < nBars; ++i )
	{
		float x1 = barX - spreadX;
		float x2 = barX + spreadX;
		float y = height_ * spectrum[ i ];
		*posPtr = { x1, -y, .0f, x2, -y, .0f }; ++posPtr;
		*posPtr = { x2, y, .0f, x1, y, .0f }; ++posPtr;
		barX += barStep;
	}
	gdiContext->unmap( vPos_ );
}

}
