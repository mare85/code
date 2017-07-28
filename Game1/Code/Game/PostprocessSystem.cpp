#include "PostprocessSystem.h"
#include <Graphics/GdiContext.h>
#include <Graphics/RenderContext.h>
#include <Graphics/Postprocess.h>
#include <Game/Camera.h>
#include <Game/Scene.h>
#include <Game/Menu.h>
#include <Black/GroundManager.h>
#include <Black/Editor/Editor.h>


void Game::PostprocessSystem::init(Graphics::GdiContext * gdiContext)
{
	typedef Graphics::Semantic S;
	Graphics::VertexDesc vDesc = {{S::Pos4, S::Uv4}};
	postprocessSh_ = gdiContext->createShader("assets/Shaders/Postprocess.fx", &vDesc);
	gaussianX_ = gdiContext->createShader("assets/Shaders/GaussianX.fx", &vDesc);
	gaussianY_ = gdiContext->createShader("assets/Shaders/GaussianY.fx", &vDesc);
	editorSh_ = gdiContext->createShader("assets/Shaders/Editor.fx", &vDesc);
	for (unsigned int i = 0; i < 6; ++i)
	{
		//rt_[i] = gdiContext->createRenderTarget(852, 480, 16, Graphics::eR32);
		rt_[i] = gdiContext->createRenderTarget(gdiContext->getWidth(), gdiContext->getHeight(), 16 * 3, Graphics::eR32);
	}
}
	
float depthPhase = .0f;

void Game::PostprocessSystem::draw(Game::Scene * scene, Graphics::GdiContext * gdiContext, float fadeValue, float time, const vmath::Vector3 &waveCenter)
{
	const Game::Camera* cam = scene->getActiveCamera();
	//depth_ = .55f + .25f * sin(depthPhase);
	depthPhase += .04f;
	if (depthPhase > 6.283f)
	{
		depthPhase -= 6.283f;
	}
	Graphics::RenderContext rctx;
	if (cam)
	{
		float aspect = 16.0f / 9.0f;

		float fade = depth_ - floor(depth_ );
		int nlayers = Black::GroundManager::eNLayers;
		int layer0 = ((int)((depth_ - 1) + nlayers)) % nlayers;
		int layer1 = ( layer0 + 1 + nlayers) % nlayers;
		int layer2 = ( layer0 + 2 + nlayers) % nlayers;
		int layer3 = ( layer0 + 3 + nlayers) % nlayers;

		float fade0 = .75f + fade * .25f;
		float fade1 = .5f + fade * .25f;
		float fade2 = .25f + fade * .25f;
		float fade3 = fade * .25f;
		rctx.view_ = cam->getView();
		rctx.xWrapEnabled_ = cam->isXWrap();
		rctx.xWrap_ = cam->getXWrap();
		rctx.projection_= cam->getProjection( aspect );
		rctx.user0_ = vmath::Vector4( time, waveCenter.getX(), waveCenter.getY(), waveCenter.getZ());

		_RenderLayer(scene, gdiContext, &rctx, fade0, layer0, 2);
		_RenderLayer(scene, gdiContext, &rctx, fade1, layer1, 3);
		_RenderLayer(scene, gdiContext, &rctx, fade2, layer2, 4);
		_RenderLayer(scene, gdiContext, &rctx, fade3, layer3, 0);
		
		gdiContext->setTextureRenderTarget(rt_[5]);
		float charScale = getScaleByFade(.5f);
		rctx.projection_ = cam->getProjection(aspect) *
			vmath::Matrix4::scale(vmath::Vector3(charScale, charScale, 1.0f)) *
			vmath::Matrix4::translation(vmath::Vector3(.0f, -.2f, 0.0f));
		
		gdiContext->setClearColor(0.0f, 0.0f, 0.0f); gdiContext->clearRender();
		scene->renderLayer(gdiContext, &rctx, 100);

		gdiContext->resetRenderTarget();

		struct __CBDATA {
			vmath::Vector4 fade;
			vmath::Vector4 backColorUp_;
			vmath::Vector4 backColorDown_;
		} cbData;

		cbData.fade = vmath::Vector4(fade, .0f, .0f, fadeValue);
		cbData.backColorUp_ = vmath::Vector4(backColorUp_, 1.0f);
		cbData.backColorDown_ = vmath::Vector4(backColorDown_, 1.0f);
		//vmath::Vector4 cbData = 
		gdiContext->setClearColor(0.0f, 0.0f, 0.0f); gdiContext->clearRender();
		rt_[2]->draw(postprocessSh_, gdiContext, &cbData, rt_[3], rt_[4], rt_[0], rt_[5]);
	}
}

void Game::PostprocessSystem::drawEditor(Game::Scene* scene, Graphics::GdiContext* gdiContext)
{
	const Game::Camera* cam = scene->getActiveCamera();
	const float aspect = 16.0f / 9.0f;
	Graphics::RenderContext rctx;
	rctx.inEditor_ = true;
	rctx.view_ = cam->getView();
	rctx.xWrapEnabled_ = cam->isXWrap();
	rctx.xWrap_ = cam->getXWrap();
	float scale = getScaleByFade(.5f );
	rctx.projection_ = cam->getProjection(aspect) * vmath::Matrix4::scale( vmath::Vector3(scale, scale, 1.0f) );
	rctx.user0_ = vmath::Vector4(.0f);
	gdiContext->setTextureRenderTarget(rt_[0]) ;
	gdiContext->setClearColor(0.0f, 0.0f, 0.0f); gdiContext->clearRender();
	Black::Editor::Editor::RenderSector( gdiContext, &rctx );
	gdiContext->setTextureRenderTarget(rt_[1]);
	gdiContext->setClearColor(0.0f, 0.0f, 0.0f); gdiContext->clearRender();
	Black::Editor::Editor::RenderSelection(gdiContext, &rctx);

	gdiContext->resetRenderTarget();
	rt_[0]->draw(editorSh_, gdiContext, nullptr, rt_[1]);
	scene->renderLayer( gdiContext, &rctx, 101 );
}

void Game::PostprocessSystem::release(Graphics::GdiContext * gdiContext)
{
	gdiContext->releaseShader(postprocessSh_);
	gdiContext->releaseShader(gaussianX_);
	gdiContext->releaseShader(gaussianY_);
	gdiContext->releaseShader(editorSh_);
	for (unsigned int i = 0; i < 5; ++i)
	{
		gdiContext->releaseRenderTarget(rt_[i]);
	}
}

void Game::PostprocessSystem::_RenderLayer(Game::Scene * scene, Graphics::GdiContext * gdiContext, Graphics::RenderContext* rctx, float fade, unsigned int layerIndex, unsigned int target)
{

	float yTranslation = fade * 3.0f - 1.5f;
	float scale = getScaleByFade(fade);
	float radius = (fade - .5f) * 5.0f;
	float waveFactor = fade - .5f;
	waveFactor = 1.0f / ( 100.0f * waveFactor * waveFactor + 1.0f) * waveAmp_;

	float xPixel[] = { radius / gdiContext->getWidth(), 0,0,0 };
	float yPixel[] = { radius / gdiContext->getHeight(), 0,0,0 };

	//layer0(geometry)->buff0;
	Graphics::RenderContext rctx2;
	rctx2.view_ = rctx->view_;
	rctx2.projection_
		= rctx->projection_
		* vmath::Matrix4::scale(vmath::Vector3(scale, scale, 1.0f))
		* vmath::Matrix4::translation(vmath::Vector3(.0f, yTranslation, .0f));
	rctx2.user0_ = rctx->user0_;
	rctx2.user0_.setW( waveFactor * rctx2.user0_.getW() );
	rctx2.xWrapEnabled_ = rctx->xWrapEnabled_;
	rctx2.xWrap_ = rctx->xWrap_;
	gdiContext->setTextureRenderTarget(rt_[0]) ;
	gdiContext->setClearColor(0.0f, 0.0f, 0.0f); gdiContext->clearRender();
	scene->renderLayer(gdiContext, &rctx2, layerIndex);

	//buff0->gaussX->buff1
	gdiContext->setTextureRenderTarget(rt_[1]);
	gdiContext->setClearColor(1.0f, 1.0f, 1.0f); gdiContext->clearRender();
	rt_[0]->draw(gaussianX_, gdiContext, xPixel);


	if (fade < .5f)
	{
		//buff1->gaussY->buff0
		gdiContext->setTextureRenderTarget(rt_[0]);
		gdiContext->setClearColor(1.0f, 1.0f, 1.0f); gdiContext->clearRender();
		rt_[1]->draw(gaussianY_, gdiContext, yPixel);

		xPixel[0] *= 3;
		yPixel[0] *= 3;

		//buff0->gaussX->buff1
		gdiContext->setTextureRenderTarget(rt_[1]);
		gdiContext->setClearColor(1.0f, 1.0f, 1.0f); gdiContext->clearRender();
		rt_[0]->draw(gaussianX_, gdiContext, xPixel);
	}
	//buff1->gaussY->buff2
	gdiContext->setTextureRenderTarget(rt_[target]);
	gdiContext->setClearColor(1.0f, 1.0f, 1.0f); gdiContext->clearRender();
	rt_[1]->draw(gaussianY_, gdiContext, yPixel);
}

void Game::PostprocessSystem::updateDepth(float dt)
{
	Black::GroundManager* gm = Black::GroundManager::getInstance();
	if (gm)
	{
		int target = gm->getLayerIndex();
		if (target < 0)
		{
			target += Black::GroundManager::eNLayers;
		}
		else if (target > Black::GroundManager::eNLayers - 1)
		{
			target -= Black::GroundManager::eNLayers;
		}
		float diff = target - depth_;
		if (diff > Black::GroundManager::eNLayers / 2)
		{
			diff -= Black::GroundManager::eNLayers;
		}
		else if (diff < -Black::GroundManager::eNLayers / 2)
		{
			diff += Black::GroundManager::eNLayers;
		}
		if( abs( diff ) > .1f )
		{
			waveAmp_ = max(waveAmp_ - dt * 2.0f, .0f );
		}
		else
		{
			waveAmp_ = min(waveAmp_ + dt * 2.0f, 1.0f );
		}
		
		if (diff < .0f )
		{
			//float diff = depth_ - target;
			float speed = max(-diff * (1.0f + diff) * 8.0f, .0f) + .05f;
			depth_ = depth_ - speed * dt;
		}
		else if (diff > .0f)
		{
			//float diff = target - depth_;
			float speed = max(diff * (1.0f - diff) * 8.0f, .0f) + .05f;
			depth_ = depth_ + speed * dt;
		}
		if (depth_ > Black::GroundManager::eNLayers - .5f)
		{
			depth_ -= Black::GroundManager::eNLayers;
		}
		else if (depth_ <= .5f)
		{
			depth_ += Black::GroundManager::eNLayers;
		}
	}
}

float Game::PostprocessSystem::getScaleByFade(float fade)
{
	return 3.4f - 2.95f * sqrt(fade);
}
