#pragma once

#include <Util/Utils.h>
#include <windows.h>

namespace Graphics
{
	class GdiContext;
	class RenderTarget;
	class RenderContext;
	class Shader;
}

namespace Game
{
	class Scene;
	class Menu;

	class PostprocessSystem
	{
		vmath::Vector3 backColorUp_ = vmath::Vector3(-.1f, .6f, .7f);
		vmath::Vector3 backColorDown_ = vmath::Vector3(.7f, .35f, -.1f);
	public:
		Graphics::RenderTarget* rt_[6];
		Graphics::Shader* gaussianX_;
		Graphics::Shader* gaussianY_;
		Graphics::Shader* postprocessSh_;
		Graphics::Shader* editorSh_;
		float depth_;
		float waveAmp_ = .0f;
		PostprocessSystem() { 
			memset(this, 0, sizeof(PostprocessSystem)); depth_ = 5.0f; 
			backColorUp_ = vmath::Vector3(-.1f, .6f, .7f);
			backColorDown_ = vmath::Vector3(.7f, .35f, -.1f);
		}
		void init(Graphics::GdiContext* gdiContext);
		void draw(Game::Scene* scene, Graphics::GdiContext* gdiContext, float fadeValue, float time, const vmath::Vector3 &waveCenter);
		void drawEditor(Game::Scene* scene, Graphics::GdiContext* gdiContext);
		void release(Graphics::GdiContext* gdiContext);
		void _RenderLayer(Game::Scene* scene, Graphics::GdiContext* gdiContext, Graphics::RenderContext* rctx, float fade, unsigned int layerIndex, unsigned int target);
		void updateDepth(float dt);
		float getScaleByFade(float fade);
		void setBackColorUp(vmath::Vector3 color) { backColorUp_ = color; }
		void setBackColorDown(vmath::Vector3 color) { backColorDown_ = color; }


	};
}
