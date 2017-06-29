#pragma once
#include <Game/Updater.h>
#include <Input/MenuInput.h>
#include <Black/CharacterObject.h>
namespace Game
{
	class Transform;
	class Text;
	class Scene;
}

namespace Black { namespace Editor {


	class CharacterAnimEditor : Game::Updater
	{
		enum
		{
			nMaxMenuItems = 30,
			nMaxKeys =  200,
			kFilenameSize = 300
		};

		struct _DirectoryPath
		{
			char path_[250];
			char label_[40];
		} directoryPaths_[50];
		unsigned int nPaths_ = 0;

		Game::Text* menuText_[nMaxMenuItems ];
		Game::Transform* menuTransforms_[ nMaxMenuItems ];
		CharacterObject* characterObject_ = nullptr;
		Game::Scene* scene_  = nullptr;
		Input::MenuInput menuInput_;
		unsigned int numKeys_= 0;
		char filename_[kFilenameSize];
		unsigned int nMenuItems_ = 0;
		unsigned int curMenu_ = 0;

		float tottalTime_ = .0f;
		bool loadMode_ = false;
		
		float currentAngles_[ CharacterObject::eBoneCount ];
		float currentOffsetX_ = .0f;
		float currentOffsetY_ = .0f;
		bool isPlaying_ = false;
		bool inKey_ = false;
		bool keyboardMode_ = false;
		char keyInput_[256];
		unsigned int curKeyIndex_ = 0;
		unsigned int nLoadFiles_ = 0;
		unsigned int levelId_ = 1;
		struct  _Key
		{
			float angles_[ CharacterObject::eBoneCount ];
			float basePosX_ = .0f;
			float basePosY_ = .0f;
			float time_ = .0f;
			void zeroValues() { 
				for( unsigned int i = 0 ;i < CharacterObject::eBoneCount ; ++i ) { angles_[i] = .0f; }
				basePosX_ = .0f;
				basePosY_ = .0f;
			}
		} keys_[nMaxKeys];
		_Key curValues_;
		void _Evaluate();
		void _AddKey();
		void _CopyKey(_Key& dst, _Key& src);
		void _DeleteKey() { _DeleteKey(curKeyIndex_); }
		void _DeleteKey(unsigned int index);
		void _SaveAnim();
		void _LoadAnim();

		void updateKeyInput(const Game::UpdateContext* uctx);
		
		void _GetAllAnimFilenames();
		void _UpdateLoadMode(const Game::UpdateContext * uctx);

		unsigned int _FindKeyIndex() {
			float t = curValues_.time_;
			unsigned int returnKey = 0;
			while (keys_[returnKey].time_ <= t && returnKey < numKeys_)
			{
				++returnKey;
			}
			--returnKey;
			inKey_ = keys_[returnKey].time_ == t;
			
			curKeyIndex_ = returnKey;
			return returnKey;
		}
	public:
		CharacterAnimEditor( Game::Scene* scene );
		~CharacterAnimEditor();
		
		void start() {}
		void update(const Game::UpdateContext* uctx);
		void stop() {}
	};

}}
