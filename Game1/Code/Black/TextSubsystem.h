#pragma once

#include <Game/Updater.h>
#include <Util/Utils.h>
#include <Util/RandomGenerator.h>
#include <Black/TextIndex.h>
namespace Game
{
	class Transform;
	class Text;
	class Scene;
}
namespace Black
{

	class TextSubsystem : public Game::Updater
	{
		enum {
			nMaxTexts = 10,
			nTextLength = 300
		};
		Util::RandomGenerator gen;
		Game::Transform* transforms_[nMaxTexts];
		Game::Text* textObjs_[nMaxTexts];
		Game::Scene* scene_ = nullptr;
		float time_[nMaxTexts];
		float progress_[nMaxTexts];
		vmath::Vector3 positions_[nMaxTexts];
		vmath::Vector3 velocities_[nMaxTexts];
		char textBuffer[nMaxTexts* nTextLength];
		char* texts_[nMaxTexts];
		unsigned int num_ = 0;
		float fadeTime_ = 1.2f;
		void _removeText(unsigned int i);
		TextlevelIndex textIndex_;
	public:
		TextSubsystem(Game::Scene* scene, unsigned int levelId, const char* lang = "pl", const char* fontName = "assets/fonts/arial");
		~TextSubsystem();
		void start() {};
		void update(const Game::UpdateContext* uctx);
		void stop() {};
		void addText(const char* text, float time_ );
		void addText(const char* filenameAndLine);
		
		static TextSubsystem* Instance();
	};


}