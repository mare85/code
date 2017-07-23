#pragma once

#include <Game/Object.h>
namespace Black
{
	class Event : public Game::Object
	{
		float width_ = .5f;
		char scriptPath[300];
		bool enabled_ = true;
		int disableOnEnter = 0;
		int isText_ = 0;

		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
	public:
		Event(const char* name);
		~Event() {}
		const char* getScript() { return scriptPath; }
		void loadData(Graphics::GdiContext* gdiContext) override;
		void unloadData(Graphics::GdiContext* gdiContext) override;
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext) override;
		void serialize(unsigned char* buffer, unsigned int* outBytes, unsigned int maxBytes) override;
		bool hasSerialize() override { return true; }
		bool isSelectable() override { return true; }
		void getBounds(float* minVal, float* maxVal);
		bool isEnabled() { return enabled_; }
		void enable() { enabled_ = true; }
		void disable() { enabled_ = false; }
		bool isDisabledOnEnter() { return disableOnEnter != 0;  }
		bool isText() const { return isText_ != 0;  }


	};


}