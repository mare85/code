#pragma once

#include <Game/Object.h>
#include <Game/Updater.h>
#include <Graphics/GdiContext.h>
#include <Graphics/Texture.h>
#include <Graphics/Shader.h>


namespace MusicRoom
{
	class Character;

	class WallsUpdater
		: public Game::Updater
	{
		Character* character_ = nullptr;;
		bool onOff_[128];
		float onOffLerp_[128];
		float waveProgress_ = .0f;
		float waveLerp_[128];
		float bpm_ = 90.0f;
	public:
		WallsUpdater(Character* character ) : character_( character ) {}
		~WallsUpdater(){}
		void start();
		void update( const Game::UpdateContext* uctx );
		void stop();
	};

	class Walls
		: public Game::Object
	{
	public:
		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void updateGfx(Graphics::GdiContext* gdiContext);
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		Walls(const char* name );
		~Walls() {};
	protected:
		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuffPosUv_ = nullptr;
		Graphics::Buffer* vBuffCol_ = nullptr;
		Graphics::Buffer* iBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		Graphics::Texture* texture_ = nullptr;

		Graphics::Shader* shFloor_ = nullptr;
		Graphics::Buffer* vBuffFloorPos_ = nullptr;
		Graphics::Buffer* vBuffFloorUv_ = nullptr;
		Graphics::Buffer* iBuffFloor_ = nullptr;
		Graphics::Texture* textureFloor_ = nullptr;
	};


}

