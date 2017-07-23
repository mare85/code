#pragma once
#include <Game/Updater.h>
#include <Game/Scene.h>
#include <Game/Object.h>
#include <Game/Text.h>
#include <Graphics/Shader.h>
#include <Graphics/GdiContext.h>
#include <Input/MenuInput.h>
#include <Util/Utils.h>
#include <Black/GroundManager.h>
#include <Black/Editor/EditorMenu.h>
#include <Game/ParamSet.h>
namespace Black
{
	namespace Editor
	{
		class Editor
			: public Game::Updater
		{
			enum {
				maxSelection = 30
			};
			enum class ObjectType {
				None,
				Tree,
				Grass,
				Sprite,
				Event
			};
			struct _CopyPaste
			{
				//Game::ParamSet* ps_;

				ObjectType ty_ = ObjectType::None;
				char stringParam_[250];
				unsigned char bufferValues_[4096];
			} copyPaste_;

			friend class add;
			friend class EditorMenu;
			vmath::Vector3 position_;
			Game::Scene* scene_;
			int layerId_;
			bool inEditionMode_ = false;

			enum
			{
				eSectorSize = 20,
			};
			struct _DirectoryPath
			{
				char path_[250];
				char label_[40];
			} directoryPaths_[50];
			unsigned int nPaths_ = 0;

			void _GetAllJpgs(const char * dirname);
		
			EditorMenu menu_;

			struct _Sector
			{
				unsigned int hashes_[eSectorSize];
				//Game::ParamSet params_[eSectorSize];
				unsigned int nHashes;

			} sectors_[GroundManager::eNLayers * GroundManager::eNSectors];
			 
			unsigned int currentSector_ = 0;
			unsigned int selectedIndex_ = 0;
			unsigned int removeHashList[eSectorSize];
			unsigned int nToRemove = 0;
			bool keyboardMode_ = false;
			void _RenderSector(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
			void _RenderSelection(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);

			_Sector* _GetCurrentSector();
			char keyInput_[250];

		public:
			Editor(Game::Scene* scene);
			~Editor();
			void start() override;
			void update(const Game::UpdateContext* uctx) override;
			void updateKeyInput(const Game::UpdateContext* uctx);
			void stop() override;

			static void SetPosition(vmath::Vector3 pos);
			static void SetLayerID(int val);
			static void Add(Game::Object* obj);
			static void RequestRemoveSelected();
			static void RemoveRequested(Graphics::GdiContext* gdi);
			static unsigned int GetSectorId(unsigned int layer, float x);
			static void RenderSector(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
			static void RenderSelection(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
			static Game::Object* GetSelected();
			static bool IsInEditionMode();
			static Editor* GetInstance();

		};

	}
}