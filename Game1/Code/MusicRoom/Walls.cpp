#include "Walls.h"
#include <Game/Transform.h>
#include <Util/RandomGenerator.h>
#include <MusicRoom/Character.h>
#include <Util/DebugDraw.h>
#include <Input/Constants.h>
#include <Input/Listener.h>
#include <Sound/System.h>


float __lightness[16 * 4];




void MusicRoom::Walls::loadData(Graphics::GdiContext* gdiContext)
{
	Graphics::VertexDesc vDescPos = Graphics::VertexDesc::get(Graphics::POS3COL3UV2);
	sh_ = gdiContext->createShader("assets/Shaders/Walls.fx", &vDescPos, "Walls");


	Graphics::VertexDesc vDescFloor = Graphics::VertexDesc::get(Graphics::POS4UV4);
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
	const unsigned int vcount = 256 * 4 * 2 * 6;
	unsigned int bufferSize = 256 * 4 * 2 * 6 * sizeof(Graphics::VertexP4UV4);
	vmath::Vector4 points[ vcount * 2];
	vmath::Vector4 * out = points;
	for( unsigned int x = 0; x < 256; ++x )
	{
		float x0 = x - 128.0f;
		float x1 = x0 + 1.0f;
		for( unsigned int i = 0; i < 4; ++i )
		{
			float y0 = i;
			float y1 = y0 + 1.0f;
			unsigned int colIndex = ( x *4 + i ) % 64;
			float col = __lightness[colIndex];
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 1.0f); ++out;

			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 1.0f); ++out;

		}
	}
	
	vBuff_ = gdiContext->createBuffer(points, bufferSize, Graphics::eDynamicWriteVertexBuffer);
	unsigned int bufferSizeFloor = sizeof(Graphics::VertexP4UV4) * 6;
	Graphics::VertexP4UV4 pointsFloor[] = {
		{ vmath::Vector4(-roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), vmath::Vector4(128.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4(-roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), vmath::Vector4(128.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4( roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4(-roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), vmath::Vector4(128.0f, 0.0f, .0f, .0f) },
		{ vmath::Vector4( roomSizeHalf, .0f, -corridorWidthHalf, 1.0f), vmath::Vector4(0.0f, 1.0f, .0f, .0f) },
		{ vmath::Vector4( roomSizeHalf, .0f,  corridorWidthHalf, 1.0f), vmath::Vector4(0.0f, 0.0f, .0f, .0f) }
	};
	
	vBuffFloor_ = gdiContext->createBuffer(pointsFloor, bufferSizeFloor, Graphics::eDefaultVertexBuffer);

	cBuff_ = gdiContext->createBuffer(nullptr, sizeof(Graphics::ConstantBufferData), Graphics::eDefaultConstantBuffer);

}

void MusicRoom::Walls::updateGfx(Graphics::GdiContext* gdiContext)
{
	float corridorWidthHalf = 2.0f;
	const unsigned int vcount = 256 * 4 * 2 * 6;
	//vmath::Vector4 points[ vcount * 2];
	vmath::Vector4 * out = reinterpret_cast<vmath::Vector4*>( gdiContext->mapWrite( vBuff_) );
	for (unsigned int x = 0; x < 256; ++x)
	{
		float x0 = x - 128.0f;
		float x1 = x0 + 1.0f;
		for (unsigned int i = 0; i < 4; ++i)
		{
			float y0 = i;
			float y1 = y0 + 1.0f;
			unsigned int colIndex = (x * 4 + i) % 64;
			float col = __lightness[colIndex];
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 1.0f); ++out;

			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 1.0f); ++out;
			*out = vmath::Vector4(x1, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, .0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x1, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 0.0f, 1.0f); ++out;
			*out = vmath::Vector4(x0, y0, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 0.0f); ++out;
			*out = vmath::Vector4(x0, y1, -corridorWidthHalf, col); ++out;
			*out = vmath::Vector4(col, col, 1.0f, 1.0f); ++out;

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
	gdiContext->releaseBuffer(vBuffFloor_);
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
	gdiContext->drawTriangles(vBuffFloor_, 6);

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
	for( unsigned int i = 0; i < 64; ++i )
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
	waveProgress_ += bpm_ * dt * 4 / 60.0f;
	if( waveProgress_ >= 16.0f )
	{
		waveProgress_ -= 16.0f;
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
		if( target.getY() > .0f && target.getY() < 4.0f )
		{
			float y = floor( target.getY() );
			float x = floor( target.getX() );
			target.setX( x + .5f );
			target.setY( y + .5f );
			Util::DebugDraw::addPoint( target, .05f, vmath::Vector4( 1.0f, 0.0f, 1.0f, 1.0f ) );
			unsigned int ix = ( (unsigned int)( x + 256 ) )% 16;
			unsigned int iy = (unsigned int)y;
			unsigned int index = ix * 4 + iy;
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
		for( unsigned int i = 0; i < 4; ++i )
		{
			unsigned int index = i + x * 4;
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
