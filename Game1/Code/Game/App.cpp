#include "App.h"

#include <Graphics/GdiContext.h>
#include <Graphics/Store.h>
#include <Graphics/Postprocess.h>
#include <Game/Scene.h>
#include <Game/Transform.h>
#include <Game/Sprite.h>
#include <Game/Menu.h>
#include <Game/Camera.h>
#include <Black/GroundManager.h>
#include <Black/Editor/Editor.h>
#include <Black/EventSystem.h>
#include <Input/Listener.h>
#include <GameUtils/Timer.h>
#include <GameUtils/InputChecker.h>
#include <GameUtils/FpsDisplay.h>
#include <Script/MenuScriptFunctions.h>
#include <Script/MenuScript.h>
#include <Sound/System.h>
#include <Util/DebugDraw.h>

Game::App* __instance = nullptr;



Game::App::App()
{
	gdiContext_ = new Graphics::GdiContext();
	gdiContext_->setClearColor(1.0f, .7f, .3f);
	input_ = new Input::Listener();
	__instance = this;
}

Game::App::~App()
{
	if (scene_)
	{
		delete scene_;
		scene_ = nullptr;
	}
	if (menuScene_)
	{
		delete menuScene_;
		menuScene_ = nullptr;
	}
	delete gdiContext_;
	delete input_;
}


void __fadeIn() { Game::App::Instance()->fade(.0f); }

void Game::App::start(HINSTANCE appInstance, HWND windowHandle)
{
	Util::initHash();
	unsigned int width = 1280;
	unsigned int height = 720;
	Script::registerMainMenuFunctions();
	Script::initFileLoader();
	gdiContext_->startUp(appInstance, windowHandle, (depthEnabled_ ? Graphics::GdiContext::UseDepthBuffer : 0), width, height);
	Sound::startUp(windowHandle, "assets/sounds");

	Sound::loadBank("bank.bnk");
	Util::DebugDraw::init(gdiContext_);

	setAspect(gdiContext_->getWidth() * 1.0f / gdiContext_->getHeight());

	postprocess_.init(gdiContext_);
	
	input_->startUp(appInstance, windowHandle);

	loadMenu("mainMenu");
	showMenu();
	setFade(0.0f);
}

unsigned int gframeIt = 0;

void Game::App::nextFrame()
{
	++gframeIt;
	if (gframeIt == 60)
	{
		gframeIt = 0;
	}
	_UpdateDt();
	postprocess_.updateDepth(dt_);
	
	_UpdateFade( dt_ );
	Script::updateTimedFunctions( dt_ );
	input_->update();
	Game::UpdateContext uctx;

	uctx.input = input_;
	uctx.deltaTime = dt_;
	if (scene_ && !paused_)
	{
		scene_->update(&uctx);
	}
	if (menuScene_)
	{
		menuScene_->update(&uctx);
	}
	if (renderFunction_ == RenderFunction::kBlack) _RenderBlack();
	if (renderFunction_ == RenderFunction::kStandard) _Render();
	if( reloadGraphicsRequest_ )
	{
		if (menuScene_)
		{
			menuScene_->unloadData( gdiContext_ );
			menuScene_->loadData( gdiContext_ );
		}
		if( scene_ )
		{
			scene_->unloadData(gdiContext_);
			scene_->loadData(gdiContext_);
		}
		reloadGraphicsRequest_ = false;
	}
	if( Game::App::EditorEnabled() )
		Black::Editor::Editor::RemoveRequested( gdiContext_ );

	if (reloadLayerRequest_)
	{
		if (scene_)
		{
			scene_->reloadGfx(gdiContext_, reloadLayerRequest_);
		}
		reloadLayerRequest_ = -1;
	}

	if( dropMenuRequest_ )
	{
		if (menuScene_)
		{
			menuScene_->unloadData(gdiContext_);
			menuScene_->stop();
			delete menuScene_;
			menuScene_ = nullptr;

		}
		menu_ = nullptr;
		dropMenuRequest_ = false;
	}
	if( dropSceneRequest_ )
	{
		if (scene_)
		{
			scene_->unloadData(gdiContext_);
			scene_->stop();
			Black::EventSystem::ShutDown();
			delete scene_;
			
			scene_ = nullptr;
		}
		dropSceneRequest_ = false;
	}
}

void Game::App::stop()
{
	if (scene_)
	{
		scene_->unloadData(gdiContext_);
		scene_->stop();
	}
	if (menuScene_)
	{
		menuScene_->unloadData(gdiContext_);
		menuScene_->stop();
	}
	Graphics::Store::getInstance()->unloadResources(gdiContext_);
	
	input_->shutDown();
	

	postprocess_.release(gdiContext_);
	Util::DebugDraw::release(gdiContext_);
	Sound::unloadBank("bank.bnk");
	Sound::shutDown();

	gdiContext_->shutDown();

	Script::MainmenuFunctionContainer::releaseInstance();
}

void Game::App::hideMenu()
{
	if (menu_) 
		menu_->setEnabled(false);

}

void Game::App::showMenu()
{
	if (menu_) 
		menu_->setEnabled(true);
}

void Game::App::loadMenu( const char *menuName )
{
	if( menu_ || menuScene_ )
		return;
	
	char fullMenuPath[250];
	sprintf_s( fullMenuPath, "assets/scripts/%s.txt", menuName );
	
	Transform* mainCam = new Transform("mainCam");
	mainCam->setTranslation(vmath::Vector3(0.0f, 0.0f, .0f));

	menuScene_ = new Scene("menu");
	menuScene_->addTransform( mainCam );
	menuScene_->createOrtographicCamera(mainCam->getHashName(), 1.0f);

	Game::Transform* fpsTextTransf = new Game::Transform("fpsText");
	fpsTextTransf->setTranslation(vmath::Vector3(-1.7f, 0.95f, .0f));
	fpsTextTransf->setScale(vmath::Vector3(2.0f, 2.0f, 1.0f));
	Game::Text* fpsText = new Game::Text("fpsText");
	
	fpsText->setColor(vmath::Vector4(1.0f, 1.0f, .0f, 0.7f));
	fpsText->prepareFont("assets/fonts/script");
	fpsTextTransf->carryObject(fpsText);
	GameUtils::FpsDisplay* fpsDisplay = new GameUtils::FpsDisplay(fpsText);
	menuScene_->addTransform(fpsTextTransf);
	menuScene_->addUpdater(fpsDisplay);

	menu_ = new Game::Menu(menuScene_, fullMenuPath);
	menuScene_->addUpdater(menu_);
	menuScene_->start();
}


void Game::App::_UpdateDt()
{
	DWORD time = GetTickCount();
	dt_ = (time - lastTime_) *.001f;
	if (dt_ > 10.0f || dt_ < .0f)
	{
		dt_ = lastDt_;
	}
	else
	{
		lastDt_ = dt_;
	}
	appTime_ += dt_;
	lastTime_ = time;
}

void Game::App::_Render()
{
	Graphics::RenderContext rctx, menuRctx;
	gdiContext_->setClearColor(0.0f, 0.0f, 0.0f); gdiContext_->clearRender();
	if (scene_)
	{
		if (!scene_->isLoaded())
		{
			scene_->loadData(gdiContext_);
		}

		const Game::Camera* cam = scene_->getActiveCamera();
		if (cam)
		{
			float aspect = 16.0f / 9.0f;
			rctx.view_ = cam->getView();
			rctx.xWrapEnabled_ = false;
			rctx.xWrap_ = .0f;
			rctx.projection_ = cam->getProjection(aspect);
			rctx.user0_ = vmath::Vector4(appTime_, .0f, .0f, .0f);
		}

		scene_->render(gdiContext_, &rctx);
	}

	if( menuScene_ )
	{
		if (!menuScene_->isLoaded())
		{
			menuScene_->loadData(gdiContext_);
		}
		const Game::Camera* menucam = menuScene_->getActiveCamera();
		if( menucam )
		{
			menuRctx.view_ = menucam->getView();
			menuRctx.projection_ = menucam->getProjection(aspect_);
			menuScene_->render(gdiContext_, &menuRctx);
		}
	}
	Util::DebugDraw::render(gdiContext_, &rctx);
	gdiContext_->swapRender();
}


void Game::App::_RenderBlack()
{
	Graphics::RenderContext renderContext, menuRctx;
	if (scene_)
	{
		if (!scene_->isLoaded())
		{
			scene_->loadData(gdiContext_);
		}
		postprocess_.draw(scene_, gdiContext_, fadeValue_, appTime_, waveCenter_);
		postprocess_.drawEditor(scene_, gdiContext_);

	}

	if (menuScene_)
	{
		if (!menuScene_->isLoaded())
		{
			menuScene_->loadData(gdiContext_);
		}
		const Game::Camera* menucam = menuScene_->getActiveCamera();
		if (menucam)
		{
			menuRctx.view_ = menucam->getView();
			menuRctx.projection_ = menucam->getProjection(aspect_);
			menuScene_->render(gdiContext_, &menuRctx);
		}
	}
	Util::DebugDraw::render(gdiContext_, &renderContext);
	gdiContext_->swapRender();
}


void Game::App::_UpdateFade(float dt)
{
	float fadeChange = dt * fadeSpeed_;
	if (fadeValue_ > fadeTarget_)
	{
		fadeValue_ = max(fadeValue_ - fadeChange, fadeTarget_);
	}
	else if (fadeValue_ < fadeTarget_)
	{
		fadeValue_ = min(fadeValue_ + fadeChange, fadeTarget_);
	}
}

Game::App * Game::App::Instance()
{
	return __instance;
}
