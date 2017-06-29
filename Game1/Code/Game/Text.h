#pragma once
#include <Game/Object.h>
namespace Game
{

	class Text :
		public Game::Object
	{

		struct _TextConstantBuffer
		{
			vmath::Matrix4 world_;
			vmath::Matrix4 projection_;
			vmath::Vector4 color_;
		};

		char fontName_[250];
		unsigned char* text_ = nullptr;
		vmath::Vector3 asciiPositions_[6 * 256];
		vmath::Vector3 asciiUvs_[6 * 256];
		vmath::Vector4 color_ = vmath::Vector4( 1.0f);
		vmath::Vector3 baseScale_ = vmath::Vector3(1.0f);
		float asciiWidths_[256];

		Graphics::Shader* sh_ = nullptr;
		Graphics::Buffer* vBuff_ = nullptr;
		Graphics::Buffer* cBuff_ = nullptr;
		Graphics::Texture* texture_ = nullptr;
		bool textToUpdate_ = false;
		unsigned int maxCharNumber_ = 0;
		unsigned int charNumber_ = 0;

	public:
		Text() : Object() { setLayer(128); }
		Text(const char* name) : Object(name) { setLayer(128); }
		~Text() { if (text_) delete []text_; }
		void prepareFont(const char* fontName );
		void setText(const char* src ) ;
		void setMaxCharNumber(unsigned int num);
		void loadData(Graphics::GdiContext* gdiContext);
		void unloadData(Graphics::GdiContext* gdiContext);
		void setColor(const vmath::Vector4& col) { color_ = col; }
		void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext);
		void setBaseScale(const vmath::Vector3& val) { baseScale_ = val; }
	};

}