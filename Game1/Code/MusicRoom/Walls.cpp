#include "Walls.h"
#include <Game/Transform.h>
#include <Util/RandomGenerator.h>
#include <MusicRoom/Character.h>
#include <Util/DebugDraw.h>
#include <Input/Constants.h>
#include <Input/Listener.h>
#include <Sound/System.h>


float __lightness[16 * 8];




void MusicRoom::Walls::loadData(Graphics::GdiContext* gdiContext)
{
	Sound::loadBank("aminharmonic.bnk");
	typedef Graphics::Semantic S;
	Graphics::VertexDesc vDescPos = { {S::Pos3, S::Col3, S::Uv2} };// Graphics::VertexDescDeprecated::get(Graphics::POS3COL3UV2);
	sh_ = gdiContext->createShader("assets/Shaders/Walls.fx", &vDescPos, "Walls");


	Graphics::VertexDesc vDescFloor = { {S::Pos4}, {S::Uv4 } };
	shFloor_ = gdiContext->createShader("assets/Shaders/Floor.fx", &vDescFloor, "Floor");
	Graphics::TextureDesc textureDesc;
	textureDesc.filter_ = Graphics::TextureDesc::eLinear;
	textureDesc.mode_ = Graphics::TextureDesc::eWrap;
	textureFloor_ = gdiContext->createTexture("assets/gfx/floor.png", &textureDesc, "assets/gfx/floor.png");
	shFloor_->setTexture(0, textureFloor_);

	Util::RandomGenerator gen;
	for (unsigned int i = 0; i < 64; ++i)
	{
		__lightness[i] = gen.getFloat();
	}

	float roomSizeHalf = 256.0f;
	float corridorWidthHalf = 2.0f;
	unsigned int bufferSize = 256 * 4 * 2 * 6 * sizeof(Graphics::VertexP4UV4);
	
	vBuff_ = gdiContext->createBuffer(nullptr, bufferSize, Graphics::BufferType::DynamicVertex);
	unsigned int bufferSizeFloorPos = sizeof(vmath::Vector4) * 6;
	vmath::Vector4 pointsFloorPos[] = {
		vmath::Vector4(-roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), 
		vmath::Vector4(-roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), 
		vmath::Vector4( roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), 
		vmath::Vector4(-roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), 
		vmath::Vector4( roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), 
		vmath::Vector4( roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), 
	};
	unsigned int bufferSizeFloorUv = sizeof(vmath::Vector4) * 6;
	vmath::Vector4 pointsFloorUv[] = {
		vmath::Vector4(128.0f, 0.0f, .0f, .0f),
		vmath::Vector4(128.0f, 1.0f, .0f, .0f),
		vmath::Vector4(0.0f, 1.0f, .0f, .0f),
		vmath::Vector4(128.0f, 0.0f, .0f, .0f),
		vmath::Vector4(0.0f, 1.0f, .0f, .0f),
		vmath::Vector4(0.0f, 0.0f, .0f, .0f)
	};
	
	vBuffFloorPos_ = gdiContext->createBuffer(pointsFloorPos, bufferSizeFloorPos, Graphics::BufferType::Vertex);
	vBuffFloorUv_ = gdiContext->createBuffer(pointsFloorUv, bufferSizeFloorUv, Graphics::BufferType::Vertex);

	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::BufferType::Constant);

}

void MusicRoom::Walls::updateGfx(Graphics::GdiContext* gdiContext)
{
	float corridorWidthHalf = 2.0f;
	//const unsigned int vcount = 256 * 4 * 2 * 6;
	vmath::Vector4 * out = reinterpret_cast<vmath::Vector4*>( gdiContext->mapWrite( vBuff_) );
	for (unsigned int x = 0; x < 256; ++x)
	{
		float x0 = x * .5f - 64.0f;
		float x1 = x0 + .5f;
		for (unsigned int i = 0; i < 4; ++i)
		{
			float y0 = 1.0f + .5f * (float)i;
			float y1 = y0 + .5f;
			unsigned int colIndex1 = (x * 8 + i) % 128;
			unsigned int colIndex2 = (x * 8 + i + 4) % 128;
			float col1 = __lightness[colIndex1];
			float col2 = __lightness[colIndex2];
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, corridorWidthHalf, col1); ++out;
			*out = vmath::Vector4(col1, col1, 1.0f, 1.0f); ++out;

			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, -corridorWidthHalf, col2); ++out;
			*out = vmath::Vector4(col2, col2, 1.0f, 1.0f); ++out;
		}
	}
	gdiContext->unmap( vBuff_);
}

void MusicRoom::Walls::unloadData(Graphics::GdiContext* gdiContext)
{
	gdiContext->releaseTexture(texture_);
	gdiContext->releaseShader(sh_);
	gdiContext->releaseBuffer(vBuff_);
	gdiContext->releaseBuffer(cBuff_);

	gdiContext->releaseTexture(textureFloor_);
	gdiContext->releaseShader(shFloor_);
	gdiContext->releaseBuffer(vBuffFloorPos_);
	gdiContext->releaseBuffer(vBuffFloorUv_);
	Sound::unloadBank("aminharmonic.bnk");
}

void MusicRoom::Walls::render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext)
{

	
	gdiContext->bindShader(shFloor_);
	Graphics::ConstantBufferData cbData;
	cbData.view_ = renderContext->view_;
	cbData.projection_ = renderContext->projection_;
	cbData.world_ = getTransform()->getWorldPose();
	gdiContext->updateBuffer(cBuff_, &cbData);
	gdiContext->setConstantBuffer(cBuff_);
	gdiContext->drawTriangles({ vBuffFloorPos_, vBuffFloorUv_ }, 6);

	gdiContext->bindShader(sh_);
	gdiContext->drawTriangles( vBuff_, 256 * 4 * 12);
	
}

MusicRoom::Walls::Walls(const char* name)
	: Game::Object(name)
{

}

unsigned int __objectId = 0;

void MusicRoom::WallsUpdater::start()
{
	Util::RandomGenerator gen;
	for( unsigned int i = 0; i < 128; ++i )
	{
		onOff_[ i ] = false;
		onOffLerp_[ i ] = .0f;
		waveLerp_[ i ] = .0f;
	}


	//Sound::loadBank( "assets/sounds/init.bnk");
	
}

void MusicRoom::WallsUpdater::update(const Game::UpdateContext* uctx)
{
	float dt = uctx->deltaTime;
	float prevWaveProgress = waveProgress_;
	waveProgress_ += bpm_ * dt * 4 / 60.0f;
	if( waveProgress_ >= 16.0f )
	{
		waveProgress_ -= 16.0f;
	}

	unsigned int iWaveProgress = (unsigned int)waveProgress_;
	unsigned int iPrevWaveProgress = (unsigned int)prevWaveProgress;

	if (iWaveProgress != iPrevWaveProgress)
	{
		if (onOff_[iWaveProgress * 8])
		{
			Sound::playSound("aminharmonic.bnk", "1");
		}
		if (onOff_[iWaveProgress * 8 + 1])
		{
			Sound::playSound("aminharmonic.bnk", "2");
		}
		if (onOff_[iWaveProgress * 8 + 2])
		{
			Sound::playSound("aminharmonic.bnk", "3");
		}
		if (onOff_[iWaveProgress * 8 + 3])
		{
			Sound::playSound("aminharmonic.bnk", "4");
		}

		if (onOff_[iWaveProgress * 8 + 4])
		{
			Sound::playSound("aminharmonic.bnk", "5");
		}
		if (onOff_[iWaveProgress * 8 + 5])
		{
			Sound::playSound("aminharmonic.bnk", "6");
		}
		if (onOff_[iWaveProgress * 8 + 6])
		{
			Sound::playSound("aminharmonic.bnk", "7");
		}
		if (onOff_[iWaveProgress * 8 + 7])
		{
			Sound::playSound("aminharmonic.bnk", "8");
		}
	}


	vmath::Vector3 front = character_->getFront();
	vmath::Vector3 pos = character_->getPosition();
	vmath::Vector3 fflat = front;
	vmath::Vector3 target = pos;
	fflat.setZ( .0f );
	float corridorWidthHalf = 2.0f;
	if( front.getZ() > .05f )
	{
		fflat /= front.getZ();
		target.setZ( corridorWidthHalf );
		target += ( corridorWidthHalf - pos.getZ() ) * fflat;
	}
	else if( front.getZ() < .05f)
	{
		fflat /= -front.getZ();
		target.setZ( -corridorWidthHalf );
		target += ( corridorWidthHalf + pos.getZ() ) * fflat;
	}
	float dist = length( target - pos );
	if( dist > .01f && dist < 3.0f )
	{
		if( target.getY() > 1.0f && target.getY() < 3.0f )
		{
			float y = floor( 2.0f * ( target.getY() -  1.0f ) );
			float x = floor( 2.0f * target.getX() - .5f);
			target.setX( x * .5f + .25f );
			target.setY( y * .5f + 1.25f );
			Util::DebugDraw::addPoint( target, .05f, vmath::Vector4( 1.0f, 0.0f, 1.0f, 1.0f ) );
			unsigned int ix = ( (unsigned int)( x + 256 ) )% 16;
			unsigned int iy = (unsigned int)y;
			unsigned int index = ix * 8 + iy;
			if (target.getZ() < .0f)
			{
				index += 4;
			}
			if( uctx->input->getKeyDown( Input::Key::Space ) )
			{
				onOff_[ index ] = !onOff_[ index ];
			}
		}
	}

	for( unsigned int x = 0; x < 16; ++x )
	{
		float waveVal = ( x - waveProgress_ ) / 16.0f;
		waveVal -= floor( waveVal );
		waveVal = max( waveVal * 4.0f - 3.0f, .0f );
		for( unsigned int i = 0; i < 8; ++i )
		{
			unsigned int index = i + x * 8;
			waveLerp_[ index ] = waveVal;
			if(onOff_[ index])
			{
				onOffLerp_[ index ] = min( onOffLerp_[ index ] + 3.0f * dt, 1.0f );
			}
			else
			{
				onOffLerp_[ index ] = max( onOffLerp_[ index ] - 3.0f * dt, 0.0f );
			}
			float val = onOffLerp_[ index ] * .4f + waveLerp_[ index ] * .4f + .2f;
			__lightness[ index ] = sqrt(val) * 1.25f;
		}
	}
}

void MusicRoom::WallsUpdater::stop()
{

}
