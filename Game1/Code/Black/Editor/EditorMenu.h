#pragma once

#include <Game/Updater.h>
#include <Input/MenuInput.h>

namespace Game
{
	class Transform;
	class Text;

}

namespace Black
{
	namespace Editor
	{
		class Editor;
		class EditorMenu
		{
			enum
			{
				eNTextLabels = 10
			};
		public:
			Input::MenuInput input;
			Editor* thisNode_ = nullptr;
			Game::Text* label_[10];
			Game::Transform* transform_[10];
			unsigned int nMenuItems_ = 0;
			unsigned int curMenuItem_ = 0;
			unsigned int chosen_ = 0xffffffff;

			enum State
			{
				eStateOff,
				eStateAdd,
				eStateEdit,
				eStateAddSprite,
				eStateInvalid
			} state_ = eStateOff, prevState_ = eStateOff, changeState_ = eStateInvalid;
			void init();
			void update(const Game::UpdateContext* uctx);
		};

	}

}