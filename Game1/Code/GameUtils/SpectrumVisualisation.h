#ifndef GAMEUTILS_SPECTRUMVISUALISATION_DEF
#define GAMEUTILS_SPECTRUMVISUALISATION_DEF
/*
 * (C) Copytight 2017 Marek Bielawski
 * GameUtils::SpectrumVisualisation
 */
#include <Game/Object.h>
#include <Graphics\GdiContext.h>
namespace GameUtils {

class SpectrumVisualisation
: public Game::Object
{

	Graphics::Buffer* vPos_;
	Graphics::Buffer* iBuff_;
	Graphics::Buffer* cBuff_;
	Graphics::Shader* sh_;

	float height_ = 1.0f;
	float width_ = 1.0f;
public:
	SpectrumVisualisation( const char* name, float width, float height )
		: Object( name )
		, width_( width )
		, height_( height )
	{}

	void loadData(Graphics::GdiContext* gdiContext) override;
	void unloadData(Graphics::GdiContext* gdiContext) override;
	void render(Graphics::GdiContext* gdiContext, Graphics::RenderContext* renderContext ) override;
	void updateGfx(Graphics::GdiContext* gdiContext) override;
};

}
#endif
