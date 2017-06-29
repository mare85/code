#pragma once
#include <windows.h>
#include <Util/Utils.h>
#include <Game/PostprocessSystem.h>

namespace Graphics
{
	class GdiContext;
}
namespace Input
{
	class Listener;
}
namespace Game
{
	class Scene;
	class Menu;
	namespace RenderFunction
	{
		enum _Enum
		{
			kStandard = 0,
			kBlack = 1
		};
	}

	class App
	{
		Scene* scene_ = nullptr;
		Scene* menuScene_ = nullptr;
		Menu* menu_ = nullptr;


		vmath::Vector3 waveCenter_;

		PostprocessSystem postprocess_;

		Graphics::GdiContext *gdiContext_ = nullptr;
		Input::Listener *input_ = nullptr;

		DWORD lastTime_ = 0;
		float lastDt_ = .0f;
		float dt_ = .0f;
		float appTime_ = .0f;
		float aspect_ = 1.0f;
		bool exitTriggered_ = false;
		bool dropMenuRequest_ = false;
		bool dropSceneRequest_ = false;
		bool paused_ = false;
		bool reloadGraphicsRequest_ = false;
		bool editorEnabled_ = false;
		int reloadLayerRequest_ = -1;
		RenderFunction::_Enum renderFunction_= RenderFunction::kStandard;

		float fadeValue_ = .0f;
		float fadeTarget_ = .0f;
		float fadeSpeed_ = 1.0f;

		void _UpdateDt();
		void _Render();
		void _RenderBlack();
		void _UpdateFade(float dt);
		
	public:
		static App* Instance();

		App();
		~App();
		void start(HINSTANCE appInstance, HWND windowHandle);
		void nextFrame();
		void stop();
		bool editorEnabled() const { return editorEnabled_; }
		static bool EditorEnabled() { return Instance()->editorEnabled(); }
		void setEditorEnabled(bool val) { editorEnabled_ = val; }
		void setRenderFunction(RenderFunction::_Enum rf) { renderFunction_ = rf; }
		
		void setAspect(float val) { 
			aspect_ = val; 
		}
		void requestExit() { exitTriggered_ = true;}
		void hideMenu();
		void showMenu();
		bool hasScene() { return scene_ != nullptr; }
		Scene* getScene() { return scene_;  }
		bool hasMenu() { return menuScene_ != nullptr; }
		void dropScene() { dropSceneRequest_ = true; }
		void dropMenu() { dropMenuRequest_ = true; }
		void setPaused(bool val) { paused_ = val; }
		void setWaveCenter( const vmath::Vector3 &wc ) { waveCenter_ = wc; }

		void loadMenu( const char *menuName );

		bool isExitTriggered() { return exitTriggered_;}
		void setScene(Scene* scene) { if( scene_ == nullptr ) scene_ = scene; }
		void setFade(float val) { fadeValue_ = val; fadeTarget_ = val; }
		void fade(float val, float time = 1.0f) { fadeTarget_ = val; fadeSpeed_ = 1.0f / time; }
		void requestReloadGraphics() {reloadGraphicsRequest_ = true;}
		void requestReloadLayerGraphics(int layerId) {reloadLayerRequest_ = layerId;}


		void setPPColor0(const vmath::Vector3& col) { postprocess_.setBackColorUp(col); }
		void setPPColor1(const vmath::Vector3& col) { postprocess_.setBackColorDown(col); }
	};

}