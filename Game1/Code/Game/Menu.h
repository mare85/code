#pragma once
#include <Game/Updater.h>
#include <Util/Utils.h>
#include <Input/MenuInput.h>

namespace Game
{
	class Scene;
	class Text;
	class Transform;

	class Menu
		: public Game::Updater
	{

		Input::MenuInput input_;
		Scene* scene_ = nullptr;
		Game::Text** textObjects_;
		Game::Transform** transforms_;
		char* scriptBuffer_;
		char** scriptNames_;
		float* fade_;
		float tottalFade_= .0f;
		float menuSpeed_ = 4.0f;

		bool enabled_ = false;

		unsigned int nElems_ = 0;
		unsigned int currentElem_ = 0;
		void _LoadScript( const char* filename);

		vmath::Vector4 menuColor_ = vmath::Vector4(1.0f, 1.0f, 1.0f, .3f);
		vmath::Vector4 selectionColor_ = vmath::Vector4(0.3f, 0.0f, 0.0f, 1.0f);
	public:
		Menu( Game::Scene* scene, char* scriptName );
		~Menu();
		void start() override;
		void update(const UpdateContext* uctx) override;
		void stop() override;
		void setEnabled(bool val) { enabled_ = val; }
	};


}