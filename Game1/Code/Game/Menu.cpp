#include "Menu.h"
#include <Game/Scene.h>
#include <Game/Text.h>
#include <Game/Transform.h>
#include <Input/Constants.h>
#include <Input/Listener.h>
#include <Script/MenuScript.h>


void Game::Menu::_LoadScript( const char* filename )
{
	FILE* f = nullptr;
	fopen_s( &f, filename, "r" );
	fscanf_s(f, "%d\n", &nElems_);
	textObjects_ = new Game::Text*[nElems_];
	transforms_ = new Game::Transform*[nElems_];
	scriptBuffer_ = new char[ 250 * nElems_];
	scriptNames_ = new char* [nElems_];
	for( unsigned int i = 0; i < nElems_; ++i )
	{
		scriptNames_[ i ] = scriptBuffer_ + (i*250);
	}
	fade_ = new float[ nElems_ ];
	vmath::Vector3 pos (.3f, -0.9f + nElems_ * .1f, .0f);

	for( unsigned int i = 0 ; i < nElems_; ++i )
	{
		char labelStr[255];
		char scriptName[ 255 ];
		fscanf_s(f, "%s", labelStr, 250);//, scriptName);
		fscanf_s(f, "%s", scriptName, 250);//, scriptName);
		textObjects_[ i ] = new Game::Text( labelStr );
		transforms_[ i ] = new Game::Transform( labelStr );
		textObjects_[ i ]->setText(labelStr);
		textObjects_[ i ]->prepareFont( "assets/fonts/script");
		textObjects_[i]->setBaseScale(vmath::Vector3(1.f, 1.f, 1.0f));
		fade_[ i ] = .0f;

		pos += vmath::Vector3( .0f, -.1f, .0f );
		transforms_[ i ]->carryObject( textObjects_[ i ] );
		transforms_[ i ]->setTranslation( pos );
		strcpy_s( scriptNames_[ i ],250, (const char*)scriptName);
		scene_->addTransform( transforms_[ i ] );	
	}
	fclose( f );
	

}

Game::Menu::Menu( Game::Scene* scene, char* scriptName )
	: scene_( scene )
{
	_LoadScript( scriptName );
}


Game::Menu::~Menu()
{
	delete [] textObjects_;
	delete [] scriptBuffer_;
	delete[] scriptNames_;
   	delete [] fade_;

}

void Game::Menu::start()
{
	currentElem_ = 0;
}

void Game::Menu::update(const UpdateContext * uctx)
{
	input_.feed(uctx->input, uctx->deltaTime);
	if (enabled_)
	{
		if( input_.up && currentElem_ != 0)
		{
			--currentElem_;
		}
		else if(input_.down && currentElem_ < nElems_ - 1)
		{
			++currentElem_;
		}
		if(input_.enter)
		{
			Script::callMainMenuFunction( scriptNames_[currentElem_] );
		}

		tottalFade_ = min(tottalFade_ + menuSpeed_ * uctx->deltaTime, 1.0f);
	}
	else
	{
		tottalFade_ = max(tottalFade_ - menuSpeed_ * uctx->deltaTime, 0.0f);
	}

	for( unsigned int i = 0; i < nElems_; ++i )
	{
		
		if( i == currentElem_ )
		{
			fade_[ i ] = min( fade_[ i ] + menuSpeed_ * uctx->deltaTime, 1.0f );
		}
		else
		{
			fade_[ i ] = max( fade_[ i ] - menuSpeed_ * uctx->deltaTime, .0f );
		}
		vmath::Vector4 color = vmath::lerp(fade_[i], menuColor_, selectionColor_);
		color.setW(color.getW() * tottalFade_);
			
		textObjects_[ i ]->setColor(color);
	}
}

void Game::Menu::stop()
{
}
